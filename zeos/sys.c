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

#define LECTURA 0
#define ESCRIPTURA 1

//Variables i estructures definides a altres llocs
extern int zeos_ticks;
extern struct list_head freequeue;
extern struct list_head readyqueue;
extern unsigned int global_PID;

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

int sys_ni_syscall()
{
	return -ENOSYS;
}

int sys_getpid()
{
	return current()->PID;
}

int sys_fork()
{
	//- Declarar estructuras auxiliares
	struct list_head * lChild = NULL;
	struct task_struct * tChild;
	union task_union * uChild;
	
	// - Buscar un PCB lliure
	if (list_empty(&freequeue)) return -ENOMEM;
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
				return -EAGAIN;
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
	init_stats(&(uChild->task.process_stats));
	set_quantum(tChild, current()->quantum);
	uChild->task.process_stats.remaining_ticks = current()->quantum;
	
	// Encuar el fill a la cua
	list_add_tail(&(uChild->task.list), &readyqueue);
	
	return uChild->task.PID;
}

void sys_exit()
{
	// - Alliberar DIR
	// - Alliberar TP
	// - Alliberar Frames
	free_user_pages(current());

	// - Alliberar PCB
	list_add_tail(&current()->list,&freequeue);

	//find next process to executre
	sched_next_rr();
}

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

int sys_gettime()
{
	return zeos_ticks;
}

int sys_get_stats(int pid, struct stats *st)
{
  printk("\nGetting stats\n");
  int i;

  // Comprobació de correctesa de parametres
  if (!access_ok(VERIFY_WRITE, st, sizeof(struct stats))) return -EFAULT; 
  if (pid < 0) return -EINVAL;

  // Recullida i update de dades.
  for (i = 0; i < NR_TASKS; i++)
  {
    if (task[i].task.PID == pid)
    {
		copy_to_user(&(task[i].task.process_stats), st, sizeof(struct stats));
		return 0;
    }
  }
  return -ESRCH; /*ESRCH */
}
