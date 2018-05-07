/*
 * sys.c - Syscalls implementation
 */
#include <devices.h>
#include <utils.h>
#include <io.h>
#include <mm.h>
#include <mm_address.h>
#include <sched.h>
#include <errno.h>
#include <libc.h>

#define LECTURA 0
#define ESCRIPTURA 1

//Variables i estructures definides a altres llocs
extern int zeos_ticks;
extern struct list_head freequeue;
extern struct list_head readyqueue;
extern int global_PID;

// Funcio per agafar l'ebp del pare
void * get_ebp();

int check_fd(int fd, int permissions)
{
	if (fd != 1) return -EBADF;
	if (permissions != ESCRIPTURA) return -EACCES;
	return 0;
}

int ret_from_fork()
{
	return 0;
}

/* System call error: Not defined */
int sys_ni_syscall()
{
	return -ENOSYS;
}

/* System call 1: Exit */
void sys_exit()
{
	//no podemos liberar memoria si estamos destruyendo un clon y aun quedan procesos usando el directorio
	int pos = ((int)get_DIR(current()) - (int)((page_table_entry*) &dir_pages[0]))/sizeof(dir_pages[0]);
	
	// free mem
	if(--dirCounter[pos] == 0)	
		free_user_pages(current());
	
	// free PCB
	update_process_state_rr(current(),&freequeue); 

	//flag process as dead
	current()->PID = -1;

	//find next process to executre
	sched_next_rr();
}

/* System call 2: Fork */
int sys_fork()
{
	//- Declarar estructuras auxiliares
	struct list_head * lChild = NULL;
	struct task_struct * tChild;
	union task_union * uChild;
	
	// - Buscar un PCB lliure
	if (list_empty(&freequeue)) return -EAGAIN;
	lChild = list_first(&freequeue);
	
	// - Libera ese PCB de la freequeue, convierte la task en la union
	list_del(lChild);
	tChild = list_head_to_task_struct(lChild);
	uChild = (union task_union *) tChild;

	// - Copiar pare
		//- PCB
		//- Pila
		//Copia en block de tot el PCB+pila (tractem amb la unio sencera)
		 copy_data(current(), uChild, sizeof(union task_union));
		
	// - Reservar phys_mem (per l'espai de dades, ja que codi i Kernel es compartit).
	//	- Numero de paginas a reservar? -> 20 (espacio logico de datos determinado). @->0x100000h 8 paginas para codigo, @ ->0x108000h 20 paginas para datos (simplificado for ZeOS)
			
	// - Buscar directori + TP (te 1024 entrades, utilitzades 20 data + 8 codi + 256 sistema, queden lliures 740)
	// Nou directori pel fill
		allocate_DIR(tChild);
		
	// Demanem la taula de pagines pel child
		page_table_entry * PT_Child = get_PT(&uChild->task);
		
	// Per cada pagina del segment de dades (NUM_PAG_DATA) mirem d'assignar un frame lliure
		int pg;
		for (pg = 0; pg < NUM_PAG_DATA; pg++)
		{
			int new_pg;
			new_pg = alloc_frame();
			
			// If no es possible allocatar un frame nou
			if (new_pg == -1)
			{
				// Allibera dades fins aquest punt
				for (new_pg = 0; new_pg < pg; new_pg++)
				{
					free_frame(get_frame(PT_Child, PAG_LOG_INIT_DATA + new_pg));
					del_ss_pag(PT_Child, PAG_LOG_INIT_DATA + new_pg);
				}
				/* Deallocata el PCB */
				list_add_tail(lChild, &freequeue);
				/* Retorna codi d'error, parant l'execució del fork */
				return -ENOMEM;
			}
			// Else assigna la pagina ss al fill.
			set_ss_pag(PT_Child, PAG_LOG_INIT_DATA + pg, new_pg);
		}		
	
	// - Mapear datos hijo
	// Demanem la taula de pagines pel parent
		page_table_entry * PT_Parent = get_PT(current());
	//	Per cada pagina del segment de systema i codi (NUM_PAG_KERNEL), mapejem a child.
		for (pg = 0; pg < NUM_PAG_KERNEL; pg++) set_ss_pag(PT_Child, pg, get_frame(PT_Parent, pg));
		for (pg = 0; pg < NUM_PAG_CODE; pg++) set_ss_pag(PT_Child, PAG_LOG_INIT_CODE + pg, get_frame(PT_Parent, PAG_LOG_INIT_CODE + pg));
		
	// - Copiar datos hijo
	for (pg = NUM_PAG_KERNEL + NUM_PAG_CODE; pg < NUM_PAG_KERNEL + NUM_PAG_CODE + NUM_PAG_DATA; pg++)
	{
    	set_ss_pag(PT_Parent, pg + NUM_PAG_DATA, get_frame(PT_Child, pg));
    	copy_data((void*) (pg << 12), (void*) ((pg + NUM_PAG_DATA) << 12), PAGE_SIZE);
    	del_ss_pag(PT_Parent, pg + NUM_PAG_DATA);
	}
	
	// - Desmapear e invalidar TLB
	set_cr3(get_DIR(current()));
	
	// Asignar el nou PID al Child
	uChild->task.PID = global_PID++;
	
	// - Crear ctxt de retorn
	//	- Return from fork (ebp i una adreça de retorn, ho fotem a la pila)
	// Cal modificar la pila del child des del pare
	// Volem agafar l'ebp del parent

	//Direccion de memoria de bottom de la pila.
	void * pilaPadre = &((union task_union *) current())->stack[KERNEL_STACK_SIZE];
	void * ebpPadre = get_ebp();
	unsigned int offset =  (pilaPadre - ebpPadre)/4;
	
	// volem modificar un ebp a 0.
	uChild->stack[KERNEL_STACK_SIZE - offset -1] = 0;
	
	// apilar el return from fork
	uChild->stack[KERNEL_STACK_SIZE - offset] = (unsigned int) &ret_from_fork;	
	
	// cal modificar el kernel_esp del child per on esta el nou cim de la pila pel return from fork
	uChild->task.kernel_esp = &uChild->stack[KERNEL_STACK_SIZE -offset -1];

	// Inicialitzacio de status del fill
	set_quantum(tChild, current()->quantum);
	init_stats(&tChild->process_stats);
	tChild->exec_status = ST_READY;
	
	// Encuar el fill a la cua
	list_add_tail(&(uChild->task.list), &readyqueue);
	
	return uChild->task.PID;
}

/* System call 4: Write */
int sys_write(int fd, char * buffer, int size)
{
	int tmp;
	tmp = check_fd(fd, ESCRIPTURA);
	if (tmp < 0) return tmp;
	if (buffer == NULL) return -EFAULT;
	if (size < 0) return -EINVAL;
	char bufferTmp[size];
	int i, ret;
	ret = 0;
	for ( i = 0; i < size; i = i+2048)
	{
		copy_from_user (&buffer[i], bufferTmp, min(2048, size-i));
		ret += sys_write_console (bufferTmp, min(2048, size-i));
	}
	return ret;
}

/* System call 10: Gettime */
int sys_gettime()
{
	return zeos_ticks;
}

/* System call 19: Clone */
int sys_clone(void (*function)(void), void *stack){
	if (!access_ok(VERIFY_WRITE, stack, 4) || !access_ok(VERIFY_READ, function, 4)){
		return -EFAULT;
	}
	//- Declarar estructuras auxiliares
	struct list_head * lChild = NULL;
	struct task_struct * tChild;
	union task_union * uChild;
	
	// - Buscar un PCB lliure
	if (list_empty(&freequeue)) return -EAGAIN;
	lChild = list_first(&freequeue);
	
	// - Libera ese PCB de la freequeue, convierte la task en la union
	list_del(lChild);
	tChild = list_head_to_task_struct(lChild);
	uChild = (union task_union *) tChild;
	copy_data(current(), uChild, sizeof(union task_union));

	// Asignar el nou PID al Child
	uChild->task.PID = global_PID++;

	// Inicialitzacio de status del fill
	set_quantum(tChild, current()->quantum);
	init_stats(&tChild->process_stats);
	tChild->exec_status = ST_READY;
	
	// Encuar el fill a la cua
	list_add_tail(&(uChild->task.list), &readyqueue);

	//afegir 1  al contador per aquest directori
	int pos = ((int)current()-(int)task)/sizeof(union task_union);
	dirCounter[pos]++;

	// Modificar pila para el clon
	//Direccion de memoria de bottom de la pila.
	void * pilaPadre = &((union task_union *) current())->stack[KERNEL_STACK_SIZE];
	void * ebpPadre = get_ebp();
	unsigned int offset =  (pilaPadre - ebpPadre)/4;
	
	// volem modificar l'ebp perque apunti a la nova pila.
	uChild->stack[KERNEL_STACK_SIZE - offset -1] =(unsigned int) stack;
	// apilar la funcio parametre
	uChild->stack[KERNEL_STACK_SIZE - offset] =(unsigned int) function;	
	//modificar kernel stack pointer
	uChild->task.kernel_esp = &uChild->stack[KERNEL_STACK_SIZE -offset -1];

	// Encuar el fill a la cua
	list_add_tail(&(uChild->task.list), &readyqueue);
	
	return uChild->task.PID;
}

/* System call 20: Getpid */
int sys_getpid()
{
	return current()->PID;
}

/* System call 35: Get_stats */
int sys_get_stats(int pid, struct stats *st) 
{
  //Nomes tenim dos estats per el que busquem nomes a ready o al current
	if (pid < 0) return -EINVAL;
	if (st == NULL) return -EFAULT;
	if (!access_ok(VERIFY_WRITE, st, sizeof(struct stats))) return -EFAULT; 
	if (pid == current()->PID) 
	{      		
		copy_to_user(&current()->process_stats, st, sizeof(struct stats));
		return 0;
	}
	int i;
	for (i = 0; i < NR_TASKS; i++) {
		if (task[i].task.PID == pid) 
		{
      		task[i].task.process_stats.remaining_ticks = current()->process_stats.remaining_ticks;
      		copy_to_user(&(task[i].task.process_stats), st, sizeof(struct stats));
      		return 0;
    	}
	}
	return -ESRCH;
}

/* Semaphore functions */
/* System call 21:  Sem_init */
int sys_sem_init(int n_sem, unsigned int value)
{	
	//1:id of sem, 2: intitial value of counter
	printk("init\n");
	
	//check correct id
	if(n_sem < 0 || n_sem > 19 || semaphores[n_sem].ownerPID >= 0) return -EINVAL;
	semaphores[n_sem].ownerPID = current()->PID;
	semaphores[n_sem].counter = value;
	INIT_LIST_HEAD(&semaphores[n_sem].queue);
	return 0;
}

/* System call 22:  Sem_wait */
int sys_sem_wait (int n_sem)
{
	printk("wait\n");
	if(n_sem < 0 || n_sem > 19 || semaphores[n_sem].ownerPID < 0) return -EINVAL;
	if(semaphores[n_sem].counter <= 0)
	{
		update_process_state_rr(current(), &semaphores[n_sem].queue);
		sched_next_rr();
		if(semaphores[n_sem].ownerPID<0)
			return -1;
	}
	else {
		--semaphores[n_sem].counter;
	}
	return 0;
}


/* System call 23:  Sem_signal */
int sys_sem_signal (int n_sem)
{
	printk("signal\n");
	if(n_sem < 0 || n_sem > 19 || semaphores[n_sem].ownerPID < 0) return -EINVAL;
	if(list_empty(&semaphores[n_sem].queue)) {
		++semaphores[n_sem].counter;
	}
	else {
		struct list_head *first = list_first(&semaphores[n_sem].queue);
		//list_del(first);
		struct task_struct *t = list_head_to_task_struct(first);
		update_process_state_rr(t, &readyqueue);
	}
	return 0;
}


/* System call 24:  Sem_destroy */
int sys_sem_destroy (int n_sem)
{
	printk("destroy\n");
	if(n_sem < 0 || n_sem > 19 || semaphores[n_sem].ownerPID < 0) return -EINVAL;
	if(current()->PID == semaphores[n_sem].ownerPID)
	{	//nomès si sóc l'amo del semafor el puc matar
		while(!list_empty(&semaphores[n_sem].queue))
		{	//mentre hi hagin procs bloquejats, els anem alliberant
			struct list_head *first = list_first(&semaphores[n_sem].queue);
			struct task_struct *t = list_head_to_task_struct(first);
			update_process_state_rr(t, &readyqueue);
		}
		semaphores[n_sem].ownerPID = -1;
		return 0;
	}
	else
		return -EPERM;
}
