/*
 * sched.c - initializes struct for task 0 anda task 1
 */

#include <sched.h>
#include <mm.h>
#include <io.h>

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

void task_switch_asm(); // <- Comes from task_switch_asm.S.
int * get_ebp();
void change_esp(void *);
//void pop_my_ebp();

extern struct list_head blocked;
// Llistes declarades a un altre fitxer
extern struct list_head freequeue;
extern struct list_head readyqueue;
// Task structs declarats a un altre fitxer. Task1 es TEMPORAL.
extern struct task_struct * idle_task;
extern struct task_struct * task1;
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


int allocate_DIR(struct task_struct *t) 
{
	int pos;

	pos = ((int)t-(int)task)/sizeof(union task_union);

	t->dir_pages_baseAddr = (page_table_entry*) &dir_pages[pos]; 

	return 1;
}

void cpu_idle(void)
{
	__asm__ __volatile__("sti": : :"memory");

	while(1)
	{
	;
	}
}

void init_idle (void)
{
	//	1)	Get an available task_union from the freequeue to contain the characteristics of this process, delete it from freequeue, assigned to global variable idle_task
	struct list_head *lTmp = list_first(&freequeue);
	list_del(lTmp);
	idle_task = list_head_to_task_struct(lTmp);
	
	//Si necesitas la union, solo tienes que hacer el casting, ya que la misma direccion de memoria apunta tanto al task_struct como al task_union.
	union task_union *uTmp = (union task_union*)idle_task;
	
	//	2)  Assign PID 0 to the process.
	idle_task->PID = global_PID++;
	
	//	3)  Initialize field dir_pages_baseAaddr with  a  new  directory  to  store  the  process  address  space using the allocate_DIR routine.
	allocate_DIR(idle_task);
	
	//	4)  Initialize  an  execution  context  for  the  procees  to  restore  it  when  it  gets  assigned  the  cpu (see section 4.5) and executes cpu_idle.
	//direccion de cpu_idle
	uTmp->stack[KERNEL_STACK_SIZE-1] = (unsigned long) &cpu_idle;
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
  	task1 = list_head_to_task_struct(lTmp);
  	union task_union *uTmp = (union task_union*) task1;
  	
  	//	1)  Assign PID 1 to the process
  	task1->PID = global_PID++;
  	
  	//	2)  Initialize field dir_pages_baseAaddr with  a  new  directory  to  store  the  process  address  space using the allocate_DIR routine.
  	allocate_DIR(task1);
  	
  	//	3)  Complete the initialization of its address space, by using the function set_user_pages (see file mm.c)
  	set_user_pages(task1);
  	
  	//	4)  Update the TSS to make it point to the new_task system stack.
  	tss.esp0 = (DWord) &(uTmp->stack[KERNEL_STACK_SIZE]);
  	
  	//	5)  Set  its  page  directory  as  the  current  page  directory  in  the  system,  by  using  the  set_cr3 function (see file mm.c).
  	set_cr3(task1->dir_pages_baseAddr);
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

void inner_task_switch(union task_union *new){
	//Cambio a la nueva kernel stack
	tss.esp0 = (int) &(new->stack[KERNEL_STACK_SIZE]);
	set_cr3(get_DIR(&new->task));
	//Deshacer enlace dinamico, guardar el ebp, cambiar el esp para apuntar al nuevo mediante el valor de kernel_esp y volver al task switch
	current()->kernel_esp = get_ebp();
	printk("ebp agafat");
	change_esp(new->task.kernel_esp);
	//pop_my_ebp();
	printk("tot fet");

}
