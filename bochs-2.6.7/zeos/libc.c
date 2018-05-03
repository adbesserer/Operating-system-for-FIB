/*
 * libc.c SYSTEM CALL WRAPPERS
 */

#include <libc.h>
#include <types.h>
#include <errno.h>

int errno;

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

void perror()
{
	switch(errno) {
		case 9:
			write (1, "Bad file number\n", 16);
			break;
		case 13:
			write (1, "Permission denied\n", 18);
			break;
		case 14:
			write (1, "Bad address\n", 12);
			break;
		case 22:
			write (1, "Invalid argument\n", 17);
			break;
		case 38:
			write (1, "Function not implemented\n",25);
			break;
		default: 
			write (1, "Error not defined\n",18);
			break;
	}
}
