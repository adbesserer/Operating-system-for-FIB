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
	jge fin_write;

	movl $0, %edx;
	subl %eax, %edx;
	movl %edx, errno;
	movl $-1, %eax;
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
	jge fin_gettime;

	movl $0, %edx;
	subl %edx, %eax;
	movl %edx, errno;
	movl $-1, %eax;
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
	jge fin_fork;

	movl $0, %edx;
	subl %edx, %eax;
	movl %edx, errno;
	movl $-1, %eax;
	/*managed error/result*/
	//Haig de detectar PID del pare i del fill aqui? >_>
	//Fem una crida a getpid?
	//Es pot fer amb cmpl sobre 0. Si es major, es el pare, que esta retornant el PID del creat.
	//Si es el fill, sera exactament 0 i per tant ja tenim el resultat correcte.

fin_fork:
	ret
