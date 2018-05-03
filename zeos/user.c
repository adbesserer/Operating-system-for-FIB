#include <libc.h>

int pid;
char buff[3];
int tmp;
void cloneFunc();
int __attribute__ ((__section__(".text.main")))


main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */
  
  clone(&cloneFunc,(void*)0x200000);

  while(1){}
  return 0;
}
void cloneFunc(){
	buff[0]='e';
	buff[1]='o';
	buff[2]='\0';
	write(1,buff,3);
	exit();
}