/*
 * sys.c - Syscalls implementation
 */
#include <devices.h>

#include <utils.h>

#include <io.h>

#include <mm.h>

#include <mm_address.h>

#include <sched.h>

#include <errno.h>

#define LECTURA 0
#define ESCRIPTURA 1

extern int zeos_ticks;

int check_fd(int fd, int permissions)
{
  if (fd!=1) return -9; /*EBADF*/
  if (permissions!=ESCRIPTURA) return -13; /*EACCES*/
  return 0;
}

int sys_ni_syscall()
{
	return -38; /*ENOSYS*/
}

int sys_getpid()
{
	return current()->PID;
}

int sys_fork()
{
  int PID=-1;

  // creates the child process
  
  return PID;
}

int sys_write(int fd,char* buffer,int size){
//fd: file descriptor. In this delivery it must always be 1.
//buffer: pointer to the bytes.
//size: number of bytes.
//return ’ Negative number in case of error (specifying the kind of error) and
//the number of bytes written if OK.
	//check parameters 1. fd
	int tmp;
	tmp = check_fd(fd, ESCRIPTURA);
	if(tmp < 0) return tmp;
	//2. buffer
	if(buffer == NULL) return -EINVAL; //
	//3. size
	if(size < 0) return -EINVAL; //

	// copy data to/from user address space (data must be brought into system space)
	char sysBuffer [size];
	copy_from_user(buffer, sysBuffer, size);
	//implement requested service
	return sys_write_console(sysBuffer, size);

}
int sys_gettime(){
	return zeos_ticks;
}
void sys_exit()
{  
}
