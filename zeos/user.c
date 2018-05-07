#include <libc.h>

int pid;
int tmp;
void cloneFunc();

int __attribute__ ((__section__(".text.main")))
main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */
  
  //clone(&cloneFunc,(void*)0x200000);
/*
	sem_init(1,1);
  int pid = fork();
	fork();
  	char buff[5];
	itoa(getpid(),buff);
	write(1,buff,5);
  if(pid==0){
  	
  	sem_signal(1);
  	sem_wait(1);
  	sem_destroy(1);
  	cloneFunc();
  }
*/
  runjp();
  while(1){}
  return 0;
}

void cloneFunc(){
	char *buff;
	buff = "Soc un clon";
	write(1,buff,12);
	exit();
}
