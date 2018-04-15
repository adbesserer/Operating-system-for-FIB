/*
 * sched.c - initializes struct for task 0 anda task 1
 */

#include <sched.h>
#include <mm.h>
#include <io.h>
#include <libc.h>
/**
 * Container for the Task array and 2 additional pages (the first and the last one)
 * to protect against out of bound accesses.
 */
union task_union protected_tasks[NR_TASKS+2]
  __attribute__((__section__(".data.task")));

union task_union *task = &protected_tasks[1]; /* == union task_union task[NR_TASKS] */

//#if 0
struct task_struct *list_head_to_task_struct(struct list_head *l)
{
  return list_entry( l, struct task_struct, list);
}
//#endif

/* Headers prototip */
void task_switch_asm(union task_union*); // <- Comes from task_switch_asm.S.
void stack_swap(void * old, void * new);

/* Constants */
#define DEFAULT_QUANTUM 20

/* variables externes i globals */
extern struct list_head blocked;
// Llistes declarades a un altre fitxer
extern struct list_head freequeue;
extern struct list_head readyqueue;
unsigned int global_PID = 0;

/* get_DIR - Returns the Page Directory address for task 't' */
page_table_entry * get_DIR (struct task_struct *t) 
{
	return t->dir_pages_baseAddr;
}

/* get_PT - Returns the Page Table address for task 't' */
page_table_entry * get_PT (struct task_struct *t) 
{
	return (page_table_entry *)(((unsigned int)(t->dir_pages_baseAddr->bits.pbase_addr))<<12);
}

/* Allocation of directory for a task struct.*/
int allocate_DIR(struct task_struct *t) 
{
	int pos;

	pos = ((int)t-(int)task)/sizeof(union task_union);

	t->dir_pages_baseAddr = (page_table_entry*) &dir_pages[pos]; 

	return 1;
}

/* Code of cpu_idle function. */
void cpu_idle(void)
{
	__asm__ __volatile__("sti": : :"memory");

	while(1)
	{
	;
	}
}

/* Initialization of statistical data */
void init_stats(struct stats *s){
	s->user_ticks = 0;
	s->system_ticks = 0;
	s->blocked_ticks = 0;
	s->ready_ticks = 0;
	s->elapsed_total_ticks = 0;
	s->total_trans = 0;
	s->remaining_ticks = DEFAULT_QUANTUM;
}

/* Updating of statistical data from context switching */
void update_stats(unsigned long *v, unsigned long *elapsed)
{
	unsigned long current_ticks;
	current_ticks = get_ticks();
	*v += current_ticks - *elapsed;
	*elapsed = current_ticks;
}

/* Initialization of idle_task */
void init_idle (void)
{
	//	1)	Get an available task_union from the freequeue to contain the characteristics of this process, delete it from freequeue, assigned to global variable idle_task
	struct list_head *lTmp = list_first(&freequeue);
	list_del(lTmp);
	idle_task = list_head_to_task_struct(lTmp);
	
	//Si necesitas la union, solo tienes que hacer el casting, ya que la misma direccion de memoria apunta tanto al task_struct como al task_union.
	union task_union *uTmp = (union task_union*)idle_task;
	
	//	2)  Assign PID 0 to the process, initialize stats, set default quantum.
	idle_task->PID = global_PID++;
	idle_task->quantum = DEFAULT_QUANTUM;
	init_stats(&idle_task->process_stats);

	//	3)  Initialize field dir_pages_baseAaddr with  a  new  directory  to  store  the  process  address  space using the allocate_DIR routine.
	allocate_DIR(idle_task);
	
	//	4)  Initialize  an  execution  context  for  the  procees  to  restore  it  when  it  gets  assigned  the  cpu (see section 4.5) and executes cpu_idle.
	//direccion de cpu_idle
	uTmp->stack[KERNEL_STACK_SIZE-1] = (unsigned int) cpu_idle;

	//valor del registro ebp que queramos al deshacer el enlace dinamico
  	uTmp->stack[KERNEL_STACK_SIZE-2] = 0;

  	//guardar la nueva posicion del stack como valor inicial del ebp
  	idle_task->kernel_esp = &(uTmp->stack[KERNEL_STACK_SIZE-2]);
	
}

void init_task1(void)
{
	// mismo proceso que con init_idle
	//	1)	Get an available task_union from the freequeue to contain the characteristics of this process, delete it from freequeue, assigned to global variable task1
	// NOTA: Es global ahora para hacer pruebas, normalmente funcionaria como una task normal y se crearia y eliminaria de la forma tipica.
	struct list_head *lTmp = list_first(&freequeue);
  	list_del(lTmp);
  	struct task_struct *task1 = list_head_to_task_struct(lTmp);
  	union task_union *uTmp = (union task_union*) task1;
  	
  	//	1)  Assign PID 1 to the process, initialize stats, set default quantum.
  	task1->PID = global_PID++;
  	task1->quantum = DEFAULT_QUANTUM;
  	init_stats(&task1->process_stats);
  	
  	//	2)  Initialize field dir_pages_baseAaddr with  a  new  directory  to  store  the  process  address  space using the allocate_DIR routine.
  	allocate_DIR(task1);
  	
  	//	3)  Complete the initialization of its address space, by using the function set_user_pages (see file mm.c)
  	set_user_pages(task1);
  	
  	//	4)  Update the TSS to make it point to the new_task system stack.
  	tss.esp0 = (DWord) &(uTmp->stack[KERNEL_STACK_SIZE]);
  	
  	//	5)  Set  its  page  directory  as  the  current  page  directory  in  the  system,  by  using  the  set_cr3 function (see file mm.c).
  	set_cr3(get_DIR(task1));
 }

void init_sched(){
	INIT_LIST_HEAD(&freequeue);
	INIT_LIST_HEAD(&readyqueue);
	int i;
	for (i = 0; i < NR_TASKS; ++i) list_add(&task[i].task.list, &freequeue);
}

struct task_struct* current()
{
  int ret_value;
  
  __asm__ __volatile__(
  	"movl %%esp, %0"
	: "=g" (ret_value)
  );
  
  return (struct task_struct*)(ret_value&0xfffff000);
}

void inner_task_switch(union task_union *new)
{
	
	//Cambio a la nueva kernel stack
	tss.esp0 = (int) &(new->stack[KERNEL_STACK_SIZE]);
	set_cr3(get_DIR(&new->task));

	//Deshacer enlace dinamico, guardar el ebp, cambiar el esp para apuntar al nuevo mediante el valor de kernel_esp y volver al task switch
	//Guardem l'EBP de la pila actual al PCB del process
	stack_swap(&current()->kernel_esp, &(new->task).kernel_esp);
}

void sched_next_rr()
{
	
	struct list_head * lTmp;
	struct task_struct *tTmp;

  	if (!list_empty(&readyqueue))
  	{
		lTmp = list_first(&readyqueue);
    	list_del(lTmp);
		tTmp = list_head_to_task_struct(lTmp);
 	 }
 	else tTmp = idle_task;
 	
	tTmp->exec_status = ST_RUN;
	tTmp->process_stats.remaining_ticks = get_quantum(tTmp);
	
	update_stats(&(current()->process_stats.system_ticks), &(current()->process_stats.elapsed_total_ticks));
  	update_stats(&(tTmp->process_stats.ready_ticks), &(tTmp->process_stats.elapsed_total_ticks));
  	tTmp->process_stats.total_trans++;
  	task_switch_asm((union task_union*)tTmp);
	
}

void update_process_state_rr(struct task_struct *t, struct list_head *dest)
{

	if (t->exec_status != ST_RUN) list_del(&(t->list));
	if (dest != NULL){
		//control if the dest list is empty?
	   	list_add_tail(&(t->list), dest);
	   	if (dest != &readyqueue) t->exec_status = ST_BLOCKED;
		else{
	    	update_stats(&(t->process_stats.system_ticks), &(t->process_stats.elapsed_total_ticks));
	    	t->exec_status = ST_READY;
	   	}
	}
	else t->exec_status = ST_RUN;
}

int needs_sched_rr()
{	
	if(current()->process_stats.remaining_ticks == 0 && !list_empty(&readyqueue)) return 1;
	else if (current()->process_stats.remaining_ticks == 0 && list_empty(&readyqueue))
	{
		current()->process_stats.remaining_ticks = get_quantum(current());
	}
	return 0;
}

void update_sched_data_rr()
{
	--current()->process_stats.remaining_ticks;
}
 
void schedule()
{
	update_sched_data_rr();
	if(needs_sched_rr()){
		update_process_state_rr(current(), &readyqueue);
		sched_next_rr();
	}
}

void set_quantum (struct task_struct *t, int new_quantum)
{
	t->quantum = new_quantum;
}
int get_quantum (struct task_struct *t)
{
	return t->quantum;
}
