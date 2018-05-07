# 1 "wrappers.S"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 1 "<command-line>" 2
# 1 "wrappers.S"
# 1 "include/asm.h" 1
# 2 "wrappers.S" 2

.globl write; .type write, @function; .align 0; write:

 pushl %ebp;
 movl %esp, %ebp;



 pushl %ebx;



 movl 8(%ebp), %ebx;
 movl 12(%ebp), %ecx;
 movl 16(%ebp), %edx;



 movl $4, %eax;
 int $0x80;



 popl %ebx;
 popl %ebp;



 cmpl $0, %eax;
 jge fin_write
    imull $-1, %eax
    movl %eax, errno
    movl $-1, %eax


fin_write:
 ret


.globl gettime; .type gettime, @function; .align 0; gettime:

 pushl %ebp;
 movl %esp, %ebp;



 movl $10, %eax;
 int $0x80;



 popl %ebp;



 cmpl $0, %eax;
 jge fin_gettime
    imull $-1, %eax
    movl %eax, errno
    movl $-1, %eax


fin_gettime:
 ret


.globl getpid; .type getpid, @function; .align 0; getpid:

 pushl %ebp;
 movl %esp, %ebp;



 movl $20, %eax;
 int $0x80;



 popl %ebp;

 ret


.globl fork; .type fork, @function; .align 0; fork:

 pushl %ebp;
 movl %esp, %ebp;



 movl $2, %eax;
 int $0x80;



 popl %ebp;



 cmpl $0, %eax;
 jge fin_fork
    imull $-1, %eax
    movl %eax, errno
    movl $-1, %eax


fin_fork:
 ret


.globl exit; .type exit, @function; .align 0; exit:

 pushl %ebp;
 movl %esp, %ebp;



 movl $1, %eax;
 int $0x80;



 popl %ebp;

 ret


.globl get_stats; .type get_stats, @function; .align 0; get_stats:

 pushl %ebp;
 movl %esp, %ebp;



 pushl %ebx
 pushl %ecx



 movl 8(%ebp),%ebx
 movl 12(%ebp),%ecx



 movl $35, %eax;
 int $0x80;



 cmpl $0, %eax;
 jge fin_get_stats
    imull $-1, %eax
    movl %eax, errno
    movl $-1, %eax


fin_get_stats:

 popl %ecx
 popl %ebx;
 popl %ebp;

 ret


.globl clone; .type clone, @function; .align 0; clone:

 pushl %ebp;
 movl %esp, %ebp;



 pushl %ebx;



 movl 8(%ebp), %ebx;
 movl 12(%ebp), %ecx;



 movl $19, %eax;
 int $0x80;



 popl %ebx;
 popl %ebp;



 cmpl $0, %eax;
 jge fin_clone
    imull $-1, %eax
    movl %eax, errno
    movl $-1, %eax


fin_clone:
 ret


.globl sem_init; .type sem_init, @function; .align 0; sem_init:

 pushl %ebp;
 movl %esp, %ebp;



 pushl %ebx;



 movl 8(%ebp), %ebx;
 movl 12(%ebp), %ecx;



 movl $21, %eax;
 int $0x80;



 popl %ebx;
 popl %ebp;



 cmpl $0, %eax;
 jge fin_sem_init
    imull $-1, %eax
    movl %eax, errno
    movl $-1, %eax


fin_sem_init:
 ret


  .globl sem_wait; .type sem_wait, @function; .align 0; sem_wait:

 pushl %ebp;
 movl %esp, %ebp;



 pushl %ebx;



 movl 8(%ebp), %ebx;



 movl $22, %eax;
 int $0x80;



 popl %ebx;
 popl %ebp;



 cmpl $0, %eax;
 jge fin_sem_wait
    imull $-1, %eax
    movl %eax, errno
    movl $-1, %eax


fin_sem_wait:
 ret


  .globl sem_signal; .type sem_signal, @function; .align 0; sem_signal:

 pushl %ebp;
 movl %esp, %ebp;



 pushl %ebx;



 movl 8(%ebp), %ebx;



 movl $23, %eax;
 int $0x80;



 popl %ebx;
 popl %ebp;



 cmpl $0, %eax;
 jge fin_sem_signal
    imull $-1, %eax
    movl %eax, errno
    movl $-1, %eax


fin_sem_signal:
 ret


  .globl sem_destroy; .type sem_destroy, @function; .align 0; sem_destroy:

 pushl %ebp;
 movl %esp, %ebp;



 pushl %ebx;



 movl 8(%ebp), %ebx;



 movl $24, %eax;
 int $0x80;



 popl %ebx;
 popl %ebp;



 cmpl $0, %eax;
 jge fin_sem_destroy
    imull $-1, %eax
    movl %eax, errno
    movl $-1, %eax


fin_sem_destroy:
 ret
