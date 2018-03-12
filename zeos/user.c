#include <libc.h>

char buff[24];

int pid;

int __attribute__ ((__section__(".text.main")))

  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */
	char buffer [5] = "Hola";
	write(1,buffer,5);
	perror();
	//test gettime
	char t[20];
	int i;
	itoa(gettime(),t);
  	write(1, t, strlen(t));
  	perror();
  	while(1) { 

  	};
  	return 0;
}
