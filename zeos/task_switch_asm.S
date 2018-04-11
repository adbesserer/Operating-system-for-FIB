#include <asm.h>

#define SAVE_AUX_REGS \
	pushl %ebx; \
	pushl %esi; \
	pushl %edi;

#define RESTORE_AUX_REGS \
	popl %edi; \
	popl %esi; \
	popl %ebx;

ENTRY(task_switch)
	/* dynamic link */
	pushl %ebp;
	movl %esp, %ebp;

	# 1) Guarda los registros ESI, EDI y EBX que la llamada no guarda (es contexto necesario para el cambio al nuevo proceso).
	SAVE_AUX_REGS;
  	
  	# 2) llama a inner_task_switch
	pushl 8(%ebp);
	call inner_task_switch;
	addl $4,(%esp);
	
	# 3) Restaura el contexto guardado previo a la llamada
	RESTORE_AUX_REGS;

	# deshacer enlace
	popl %ebp;
	ret

ENTRY(get_ebp) # function to get the current ebp
	movl %ebp, %eax;
	ret

ENTRY(change_esp) # changes esp to the parameter passed and pop ebp
	movl 4(%ebp), %esp;
	ret

# ENTRY(pop_my_ebp)
#	movl 4(%esp), %ebp;
#	ret
