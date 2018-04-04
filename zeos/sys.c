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
  if (fd!=1) return -EBADF;
  if (permissions!=ESCRIPTURA) return -EACCES; 
  return 0;
}

int sys_ni_syscall()
{
	return -ENOSYS;
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

void sys_exit()
{
	
}

int sys_write(int fd, char * buffer, int size)
{
	int tmp;
	tmp = check_fd(fd, ESCRIPTURA);
	if (tmp < 0) return tmp;
	if (buffer == NULL) return -EFAULT;
	if (size < 0) return -EINVAL;
	char bufferTmp[size];
	int i, ret;
	ret = 0;
	for ( i = 0; i < size; i=i+2048)
	{
		copy_from_user (&buffer[i], bufferTmp, min (2048, size-i));
		ret += sys_write_console (bufferTmp, min(2048, size-i));
	}
	return ret;
}

int sys_gettime()
{
	return zeos_ticks;
}
