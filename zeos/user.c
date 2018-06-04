#include <libc.h>

int pid;
int tmp;
char buff[128];
void cloneFunc();

int __attribute__ ((__section__(".text.main")))
main(void)
{
  runjp();
  //int * point = sbrk (sizeof(int));
  //*point = 666;
  //write(1,buff,15);
  while(1){
  }
  return 0;
}
