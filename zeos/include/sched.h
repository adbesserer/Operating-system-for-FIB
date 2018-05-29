/*
 * sched.h - Estructures i macros pel tractament de processos
 */

#ifndef __SCHED_H__
#define __SCHED_H__

#include <list.h>
#include <types.h>
#include <mm_address.h>
#include <stats.h>
#include <utils.h>
#include <stat_funcs.h>

#define NR_TASKS      10
#define KERNEL_STACK_SIZE	1024

enum state_t { ST_RUN, ST_READY, ST_BLOCKED };

struct task_struct {
  int PID;			/* Process ID. This MUST be the first field of the struct. */
  page_table_entry * dir_pages_baseAddr;
  struct list_head list; /* TODO: Llista on es troba el proces actualment */
  void * kernel_esp;
  enum state_t exec_status;
  struct stats process_stats;
  int quantum;

  void *heap;
  int heapSize; //BYTES
  int heapPages;
};

union task_union {
  struct task_struct task;
  unsigned long stack[KERNEL_STACK_SIZE];    /* pila de sistema, per procés */
};

struct semaphore {
	int ownerPID; //PID de l'amo
	int counter;
	struct list_head queue;
  int semID;
};

struct semaphore semaphores[20];
extern int dirCounter[NR_TASKS];
extern union task_union protected_tasks[NR_TASKS+2];
extern union task_union *task; /* Vector de tasques */
extern struct task_struct *idle_task;

#define KERNEL_ESP(t) (DWord) &(t)->stack[KERNEL_STACK_SIZE]

#define INITIAL_ESP   KERNEL_ESP(&task[1])

/* Inicialitza les dades estadistiques de un proces */
void init_stats(struct stats *s);

/* Inicialitza les dades del proces inicial */
void init_task1(void);

/* Inicialitza el proces idle */
void init_idle(void);

/* Inicialitza l'scheduling */
void init_sched(void);

/* Retorna el PCB de la tasca actual */
struct task_struct * current();

/* Canvi de tasca */
void task_switch(union task_union*t);

/* Retorna el PCB d'un element d'una llista */
struct task_struct *list_head_to_task_struct(struct list_head *l);

/* Reserva un directori per un process */
int allocate_DIR(struct task_struct *t);

/* Retorna la taula de pagines d'un proces */
page_table_entry * get_PT (struct task_struct *t) ;

/* Retorna el directori d'un proces */
page_table_entry * get_DIR (struct task_struct *t) ;

/* Headers for the scheduling policy */
void sched_next_rr();
void update_process_state_rr(struct task_struct *t, struct list_head *dest);
int needs_sched_rr();
void update_sched_data_rr();
void schedule();
void unblock(struct task_struct *t);
void block(struct task_struct *t, struct list_head *dest);

/* Quantum related functions */
int get_quantum (struct task_struct *t);
void set_quantum (struct task_struct *t, int new_quantum);

#endif  /* __SCHED_H__ */
