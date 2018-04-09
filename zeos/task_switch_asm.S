#include <asm.h>

ENTRY(task_switch)
	/* dynamic link */
	pushl %ebp
	movl %esp, %ebp

	# 1) Guarda los registros ESI, EDI y EBX que la llamada no guarda (es contexto necesario para el cambio al nuevo proceso).
  	pushl %ebx
	pushl %esi
	pushl %edi
  	
  	# 2) llama a inner_task_switch
  	pushl 8(%ebp)
	call inner_task_switch
	addl $4,(%esp)
	
	# 3) Restaura el contexto guardado previo a la llamada
	popl %edi
	popl %esi
	popl %ebx
	
	# deshacer enlace
	popl %ebp
	ret

ENTRY(get_ebp) # function to get the current ebp
	movl %ebp, %eax
	ret

ENTRY(change_esp) # changes esp to the parameter passed
	movl 8(%ebp), %esp
	ret

ENTRY(pop_my_ebp)
	popl %ebp
	ret