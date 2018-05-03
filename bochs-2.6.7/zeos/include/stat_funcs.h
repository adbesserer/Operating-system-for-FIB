#ifndef __STAT_FUNCS__
#define __STAT_FUNCS__

#include <sched.h>
#include <stats.h>
#include <io.h>
#include <libc.h>

void user_to_sys_stats();
void sys_to_user_stats();
void sys_to_ready_stats();
void ready_to_sys_stats();
void sys_to_process_stats(struct task_struct *t);

#endif
