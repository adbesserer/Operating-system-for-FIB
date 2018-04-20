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
	movl 16(%ebp), %edx; /*size*/
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
	
	pushl %ebx
	pushl %ecx

	movl 8(%ebp),%ebx
	movl 12(%ebp),%ecx
	/* interrupt generation*/
	movl $35, %eax;
	int $0x80;
	/* interrupt generation done */


	/*recovered context*/

	/*manage error/result*/
	cmpl $0, %eax;
	jge fin_get_stats
        imull $-1, %eax
        movl %eax, errno
        movl $-1, %eax
	
fin_get_stats:
	/*recover context*/
	popl %ecx
	popl %ebx;
	popl %ebp;
	ret
	