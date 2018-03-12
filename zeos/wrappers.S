#include <asm.h>

/*asm ( assembler template 
           : output operands                  optional
           : input operands                   optional
           : list of clobbered registers      optional
           ); */
/*
+---+--------------------+
| r |    Register(s)     |
+---+--------------------+
| a |   %eax, %ax, %al   |
| b |   %ebx, %bx, %bl   |
| c |   %ecx, %cx, %cl   |
| d |   %edx, %dx, %dl   |
| S |   %esi, %si        |
| D |   %edi, %di        |
+---+--------------------+*/


ENTRY(write)
  pushl %ebp
  movl  %esp, %ebp
  /*save context*/
  pushl %ebx
  /*parameter passing*/
  movl 8(%ebp), %ebx
  movl 12(%ebp), %ecx
  movl 16(%ebp), %edx

  /*identifier of system call in eax*/
  movl $4, %eax
  /*generate interrupt*/
  int $0x80
  /*restore context*/
  popl %ebx
  popl %ebp
  /*manage error/result*/
  cmpl $0, %eax
  jge  finWr
  /*save absolute value of error in errno*/
  movl $0, %edx
  subl %edx, %eax
  movl %edx, errno
  /*put -1 in eax*/
  movl $-1, %eax
finWr:
  ret

ENTRY(gettime)
	pushl %ebp
 	movl  %esp, %ebp
  	/*identifier of system call in eax*/
  	movl $10, %eax
  	/*generate interrupt*/
  	int $0x80
  	/*manage error/result*/
  	cmpl $0, %eax
  	jge  finGt
  	/*save absolute value of error in errno*/
  	movl $0, %edx
  	subl %edx, %eax
  	movl %edx, errno
  	/*put -1 in eax*/
  	movl $-1, %eax
finGt:
  	ret