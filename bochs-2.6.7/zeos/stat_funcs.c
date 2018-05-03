#include <stat_funcs.h>

void user_to_sys_stats(){
	struct stats *st = &current()->process_stats;
	st->user_ticks += get_ticks()-st->elapsed_total_ticks;
	st->elapsed_total_ticks=get_ticks();
}

void sys_to_user_stats(){
	struct stats *st = &current()->process_stats;
	st->system_ticks +=get_ticks()-st->elapsed_total_ticks;
	st->elapsed_total_ticks=get_ticks();
}

void sys_to_ready_stats(){
	struct stats *st = &current()->process_stats;
	st->system_ticks +=get_ticks()-st->elapsed_total_ticks;
	st->elapsed_total_ticks=get_ticks();
}

void ready_to_sys_stats(){
	struct stats *st = &current()->process_stats;
	st->ready_ticks +=get_ticks()-st->elapsed_total_ticks;
	st->elapsed_total_ticks=get_ticks();
}

void sys_to_process_stats(struct task_struct * t){
	t->process_stats.system_ticks += get_ticks() - t->process_stats.elapsed_total_ticks;
	t->process_stats.elapsed_total_ticks = get_ticks();
}
