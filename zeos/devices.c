/*
 * devices.c DEVICE DEPENDENT PART OF THE SYSTEM CALLS
 */

#include <io.h>
#include <utils.h>
#include <list.h>
#include <errno.h>
#include <sched.h>

#define N 10		//tamaño del circular buffer

// Queue for blocked processes in I/O 
struct list_head blocked;
char circBuffer[N];//buffer circular para lectura del teclado
int nextToWrite=0;	//last written position of the circ buffer
int lastRead=0;		//last read position of the circ buffer
int leftToRead;
int cb_is_empty = 1;
extern struct list_head keyboardqueue;

int sys_write_console(char *buffer,int size)
{
  int i;
  
  for (i=0; i<size; i++)
    printc(buffer[i]);
  
  return size;
}

//funciones de acceso al circular buffer
int sys_read_keyboard(char* buff, int n){	//take n chars from the circbuffer
	//if there are processes blocked, go to the end of the queue
	leftToRead = n;
	if(!list_empty(&keyboardqueue) || cb_is_empty){
		block(current(),&keyboardqueue);
		sched_next_rr();
	}
	int i = 0; //i = how many chars have been read
	 
	//block the process until we have the data
	while(i < n){
		while(lastRead != nextToWrite){
			//if(buff[i] == NULL) 	return -EFAULT;

			buff[i++] = circBuffer[lastRead++];
			--leftToRead;

			if(lastRead == N) 	lastRead = 0; //if we get to the end we start again
			if(lastRead == nextToWrite) cb_is_empty=1;
		}	
		//block at start of queue
		if(i<n){
			list_add(&(current()->list),&keyboardqueue);
			sys_to_ready_stats();
			current()->exec_status = ST_BLOCKED;
			sched_next_rr();
		}
	}
	return i;	 

}
int charsInCirc(){	//torna el numero de chars que hi ha al circbuffer
	if(nextToWrite >= lastRead)
		return nextToWrite - lastRead;
	else
		return N - (lastRead - nextToWrite);

}
//para saber si el circular buffer está lleno
int cbfull(){
	return !cb_is_empty;
}
//poner 1 char en el cirbuffer
int putC(char C){	//put n chars into the circbuffer
	if(cbfull() && list_empty(&keyboardqueue)) //circbuffer full and no procs waiting
		return -ENOMEM;
	if(!cbfull()){
		if(nextToWrite == N) 	nextToWrite = 0; //if we get to the end we start again
		circBuffer[nextToWrite++] = C;
		if(lastRead == nextToWrite) cb_is_empty=1;

	}
	if(nextToWrite == N) 	nextToWrite = 0; //if we get to the end we start again
	return 0;
}
