/*
 * devices.c DEVICE DEPENDENT PART OF THE SYSTEM CALLS
 */

#include <io.h>
#include <utils.h>
#include <list.h>
#include <errno.h>
#include <sched.h>

// Queue for blocked processes in I/O 
struct list_head blocked;
char circBuffer[16];//buffer circular para lectura del teclado
int lastWritten=0;	//last written position of the circ buffer
int lastRead=0;		//last read position of the circ buffer
extern struct list_head keyboardqueue;

int sys_write_console(char *buffer,int size)
{
  int i;
  
  for (i=0; i<size; i++)
    printc(buffer[i]);
  
  return size;
}

int strlen(char *a)
{
  int i;
  i=0;
  while (a[i]!=0) i++;
  return i;
}
//funciones de acceso al circular buffer
int sys_read_keyboard(char* buff, int n){	//take n chars from the circbuffer
	//if there are processes blocked, go to the end of the queue
	if(!list_empty(&keyboardqueue)){
		block(current(),&keyboardqueue);
		sched_next_rr();
	}
	if(lastWritten==lastRead)//nothing to read
		return 0;
	int i = 0; //i = how many chars have been read
	int size = strlen(circBuffer);
	while(lastRead != lastWritten && i < n){
		if(buff[i] == NULL) 	return -EFAULT;

		buff[i++] = circBuffer[lastRead++];

		if(lastRead == size) 	lastRead = 0; //if we get to the end we start again
	}	 
	if(i<n)			//block the process until we have the data
		while(i < n){
			block(current(),&keyboardqueue);
			sched_next_rr();
			while(lastRead != lastWritten && i < n){
				if(buff[i] == NULL) 	return -EFAULT;
				buff[i++] = circBuffer[lastRead++];
				if(lastRead == size) 	lastRead = 0; //if we get to the end we start again
			}
		}
	return i;	 

}
//para saber si el circular buffer está lleno
int cbfull(){
	int size = strlen(circBuffer);
	if(lastWritten == lastRead-1 || (lastWritten == (size-1) && lastRead==0))
		return 1;
	return 0;
}
//poner 1 char en el cirbuffer
int putC(char C){	//put n chars into the circbuffer
	if(cbfull() && list_empty(&keyboardqueue)) //circbuffer full and no procs waiting
		return -ENOMEM;
	if(!cbfull()){
		int size = strlen(circBuffer);
		if(lastWritten == size) 	lastWritten = -1; //if we get to the end we start again
		circBuffer[++lastWritten] = C;
	}
	if(!list_empty(&keyboardqueue)){
		struct list_head *first = list_first(&keyboardqueue);
		struct task_struct *t = list_head_to_task_struct(first);
		unblock(t);
	}
}
