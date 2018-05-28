#include <asm.h>

ENTRY(write)
	/* dynamic link */
	pushl %ebp;
	movl %esp, %ebp;
	/* dynamic link */
	
	/*save registers to be used, typically eax, ebx, ecx */
	pushl %ebx;
	/*registers saved*/
	
	/*recover parameters*/
	movl 8(%ebp), %ebx; 	/*fd*/
	movl 12(%ebp), %ecx; 	/*buffer*/
	movl 16(%ebp), %edx; 	/*size*/
	/*recovered parameters*/
	
	/* interrupt generation*/
	movl $4, %eax;
	int $0x80;
	/* interrupt generation done */
	
	/*recover context*/
	popl %ebx;
	popl %ebp;
	/*recovered context*/
	
	/*manage error/result*/
	cmpl $0, %eax;
	jge fin_write
    imull $-1, %eax
    movl %eax, errno
    movl $-1, %eax
	/*managed error/result*/
	
fin_write:
	ret

ENTRY(read)
	/* dynamic link */
	pushl %ebp;
	movl %esp, %ebp;
	/* dynamic link */
	
	/*save registers to be used, typically eax, ebx, ecx */
	pushl %ebx;
	/*registers saved*/
	
	/*recover parameters*/
	movl 8(%ebp), %ebx; 	/*fd*/
	movl 12(%ebp), %ecx; 	/*buffer*/
	movl 16(%ebp), %edx; 	/*count*/
	/*recovered parameters*/
	
	/* interrupt generation*/
	movl $3, %eax;
	int $0x80;
	/* interrupt generation done */
	
	/*recover context*/
	popl %ebx;
	popl %ebp;
	/*recovered context*/
	
	/*manage error/result*/
	cmpl $0, %eax;
	jge fin_read
    imull $-1, %eax
    movl %eax, errno
    movl $-1, %eax
	/*managed error/result*/
	
fin_read:
	ret

ENTRY(gettime)
	/* dynamic link */
	pushl %ebp;
	movl %esp, %ebp;
	/* dynamic link */
	
	/* interrupt generation*/
	movl $10, %eax;
	int $0x80;
	/* interrupt generation done */
	
	/*recover context*/
	popl %ebp;
	/*recovered context*/
	
	/*manage error/result*/
	cmpl $0, %eax;
	jge fin_gettime
    imull $-1, %eax
    movl %eax, errno
    movl $-1, %eax
	/*managed error/result*/

fin_gettime:
	ret


ENTRY(getpid)
	/* dynamic link */
	pushl %ebp;
	movl %esp, %ebp;
	/* dynamic link */

	/* interrupt generation*/
	movl $20, %eax;
	int $0x80;
	/* interrupt generation done */

	/*recover context*/
	popl %ebp;
	/*recovered context*/
	ret


ENTRY(fork)
	/* dynamic link */
	pushl %ebp;
	movl %esp, %ebp;
	/* dynamic link */

	/* interrupt generation*/
	movl $2, %eax;
	int $0x80;
	/* interrupt generation done */

	/*recover context*/
	popl %ebp;
	/*recovered context*/

	/*manage error/result*/
	cmpl $0, %eax;
	jge fin_fork
    imull $-1, %eax
    movl %eax, errno
    movl $-1, %eax
	/*managed error/result*/

fin_fork:
	ret


ENTRY(exit)
	/* dynamic link */
	pushl %ebp;
	movl %esp, %ebp;
	/* dynamic link */

	/* interrupt generation*/
	movl $1, %eax;
	int $0x80;
	/* interrupt generation done */

	/*recover context*/
	popl %ebp;
	/*recovered context*/
	ret


ENTRY(get_stats)
/* dynamic link */
	pushl %ebp;
	movl %esp, %ebp;
	/* dynamic link */
	
	/*save registers to be used, typically eax, ebx, ecx */
	pushl %ebx
	pushl %ecx
	/*registers saved*/

	/*recover parameters*/
	movl 8(%ebp),%ebx
	movl 12(%ebp),%ecx
	/*recover parameters*/
	
	/* interrupt generation*/
	movl $35, %eax;
	int $0x80;
	/* interrupt generation done */

	/*manage error/result*/
	cmpl $0, %eax;
	jge fin_get_stats
    imull $-1, %eax
    movl %eax, errno
    movl $-1, %eax
    /*managed error/result*/
    
fin_get_stats:
	/*recover context*/
	popl %ecx
	popl %ebx;
	popl %ebp;
	/*recovered context*/
	ret
	
	
ENTRY(clone)
	/* dynamic link */
	pushl %ebp;
	movl %esp, %ebp;
	/* dynamic link */
	
	/*save registers to be used, typically eax, ebx, ecx */
	pushl %ebx;
	/*registers saved*/
	
	/*recover parameters*/
	movl 8(%ebp), %ebx; 	/*function*/
	movl 12(%ebp), %ecx; 	/*stack*/
	/*recovered parameters*/
	
	/* interrupt generation*/
	movl $19, %eax;
	int $0x80;
	/* interrupt generation done */
	
	/*recover context*/
	popl %ebx;
	popl %ebp;
	/*recovered context*/
	
	/*manage error/result*/
	cmpl $0, %eax;
	jge fin_clone
    imull $-1, %eax
    movl %eax, errno
    movl $-1, %eax
	/*managed error/result*/
	
fin_clone:
	ret


ENTRY(sem_init)
	/* dynamic link */
	pushl %ebp;
	movl %esp, %ebp;
	/* dynamic link */
	
	/*save registers to be used, typically eax, ebx, ecx */
	pushl %ebx;
	/*registers saved*/
	
	/*recover parameters*/
	movl 8(%ebp), %ebx; 	/*id of sem*/
	movl 12(%ebp), %ecx; 	/*initial value of counter*/
	/*recovered parameters*/
	
	/* interrupt generation*/
	movl $21, %eax;
	int $0x80;
	/* interrupt generation done */
	
	/*recover context*/
	popl %ebx;
	popl %ebp;
	/*recovered context*/
	
	/*manage error/result*/
	cmpl $0, %eax;
	jge fin_sem_init
    imull $-1, %eax
    movl %eax, errno
    movl $-1, %eax
	/*managed error/result*/
	
fin_sem_init:
	ret


  ENTRY(sem_wait)
	/* dynamic link */
	pushl %ebp;
	movl %esp, %ebp;
	/* dynamic link */
	
	/*save registers to be used, typically eax, ebx, ecx */
	pushl %ebx;
	/*registers saved*/
	
	/*recover parameters*/
	movl 8(%ebp), %ebx; 	/*id of sem*/
	/*recovered parameters*/
	
	/* interrupt generation*/
	movl $22, %eax;
	int $0x80;
	/* interrupt generation done */
	
	/*recover context*/
	popl %ebx;
	popl %ebp;
	/*recovered context*/
	
	/*manage error/result*/
	cmpl $0, %eax;
	jge fin_sem_wait
    imull $-1, %eax
    movl %eax, errno
    movl $-1, %eax
	/*managed error/result*/
	
fin_sem_wait:
	ret


  ENTRY(sem_signal)
	/* dynamic link */
	pushl %ebp;
	movl %esp, %ebp;
	/* dynamic link */
	
	/*save registers to be used, typically eax, ebx, ecx */
	pushl %ebx;
	/*registers saved*/
	
	/*recover parameters*/
	movl 8(%ebp), %ebx; 	/*id of sem*/
	/*recovered parameters*/
	
	/* interrupt generation*/
	movl $23, %eax;
	int $0x80;
	/* interrupt generation done */
	
	/*recover context*/
	popl %ebx;
	popl %ebp;
	/*recovered context*/
	
	/*manage error/result*/
	cmpl $0, %eax;
	jge fin_sem_signal
    imull $-1, %eax
    movl %eax, errno
    movl $-1, %eax
	/*managed error/result*/

fin_sem_signal:
	ret


  ENTRY(sem_destroy)
	/* dynamic link */
	pushl %ebp;
	movl %esp, %ebp;
	/* dynamic link */
	
	/*save registers to be used, typically eax, ebx, ecx */
	pushl %ebx;
	/*registers saved*/
	
	/*recover parameters*/
	movl 8(%ebp), %ebx; 	/*id of sem*/
	/*recovered parameters*/
	
	/* interrupt generation*/
	movl $24, %eax;
	int $0x80;
	/* interrupt generation done */
	
	/*recover context*/
	popl %ebx;
	popl %ebp;
	/*recovered context*/
	
	/*manage error/result*/
	cmpl $0, %eax;
	jge fin_sem_destroy
    imull $-1, %eax
    movl %eax, errno
    movl $-1, %eax
	/*managed error/result*/
	
fin_sem_destroy:	
	ret

ENTRY(sbrk)
	/* dynamic link */
	pushl %ebp;
	movl %esp, %ebp;
	/* dynamic link */
	
	/*save registers to be used, typically eax, ebx, ecx */
	pushl %ebx;
	/*registers saved*/
	
	/*recover parameters*/
	movl 8(%ebp), %ebx; 	/*increment*/
	/*recovered parameters*/
	
	/* interrupt generation*/
	movl $19, %eax;
	int $0x80;
	/* interrupt generation done */
	
	/*recover context*/
	popl %ebx;
	popl %ebp;
	/*recovered context*/
	
	/*manage error/result*/
	cmpl $0, %eax;
	jge fin_sbrk
    imull $-1, %eax
    movl %eax, errno
    movl $-1, %eax
	/*managed error/result*/
	
fin_sbrk:
	ret
