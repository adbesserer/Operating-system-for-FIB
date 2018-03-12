# 1 "wrappers.S"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 1 "<command-line>" 2
# 1 "wrappers.S"
# 1 "include/asm.h" 1
# 2 "wrappers.S" 2
# 21 "wrappers.S"
.globl write; .type write, @function; .align 0; write:
  pushl %ebp
  movl %esp, %ebp

  pushl %ebx

  movl 8(%ebp), %ebx
  movl 12(%ebp), %ecx
  movl 16(%ebp), %edx


  movl $4, %eax

  int $0x80

  popl %ebx
  popl %ebp

  cmpl $0, %eax
  jge finWr

  movl $0, %edx
  subl %edx, %eax
  movl %edx, errno

  movl $-1, %eax
finWr:
  ret

.globl gettime; .type gettime, @function; .align 0; gettime:
 pushl %ebp
  movl %esp, %ebp

   movl $10, %eax

   int $0x80

   cmpl $0, %eax
   jge finGt

   movl $0, %edx
   subl %edx, %eax
   movl %edx, errno

   movl $-1, %eax
finGt:
   ret
