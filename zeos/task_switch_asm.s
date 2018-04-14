# 1 "task_switch_asm.S"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 1 "<command-line>" 2
# 1 "task_switch_asm.S"
# 1 "include/asm.h" 1
# 2 "task_switch_asm.S" 2
# 13 "task_switch_asm.S"
.globl task_switch; .type task_switch, @function; .align 0; task_switch:

 pushl %ebp;
 movl %esp, %ebp;

 # 1) Guarda los registros ESI, EDI y EBX que la llamada no guarda (es contexto necesario para el cambio al nuevo proceso).
 pushl %ebx; pushl %esi; pushl %edi;;

   # 2) llama a inner_task_switch
 pushl 8(%ebp);
 call inner_task_switch;
 addl $4,(%esp);

 # 3) Restaura el contexto guardado previo a la llamada
 popl %edi; popl %esi; popl %ebx;;

 # deshacer enlace
 popl %ebp;
 ret

# .globl pop_my_ebp; .type pop_my_ebp, @function; .align 0; pop_my_ebp:
 # movl 4(%esp), %ebp;
# ret

.globl do_the_stuff; .type do_the_stuff, @function; .align 0; do_the_stuff: # 1er param: old kernel stackpointer, 2ndo param: new kernel stackpointer
 pushl %ebp;
 movl %esp, %ebp;
 movl 8(%ebp), %ecx # ecx = &old
 movl 12(%ebp), %edx # edx = &new
 movl %ebp, (%ecx)
 movl (%edx), %esp
 popl %ebp
 ret
