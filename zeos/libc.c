/*
 * libc.c 
 */

#include <libc.h>
#include <errno.h>
#include <types.h>

int errno = 0;

void itoa(int a, char *b)
{
  int i, i1;
  char c;
  
  if (a==0) { b[0]='0'; b[1]=0; return ;}
  
  i=0;
  while (a>0)
  {
    b[i]=(a%10)+'0';
    a=a/10;
    i++;
  }
  
  for (i1=0; i1<i/2; i1++)
  {
    c=b[i1];
    b[i1]=b[i-i1-1];
    b[i-i1-1]=c;
  }
  b[i]=0;
}

int strlen(char *a)
{
  int i;
  
  i=0;
  
  while (a[i]!=0) i++;
  
  return i;
}

void perror(){
	switch(errno){
		case 0:
			break;
		case EBADF:
			write(1,"Bad file number\n",17);
			break;
		case ENOSYS:
			write(1,"Function not implemented\n",26);
			break;
		case EINVAL:
			write(1,"Invalid argument\n",18);
			break;	
		case EACCES:
			write(1,"Permission denied\n",19);
			break;
		default:
			write(1,"Unspecified error\n",19);
	}
}