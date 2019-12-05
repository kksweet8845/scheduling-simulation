#ifndef TASK_H
#define TASK_H

#include "typedefine.h"
#include "config.h"
#include "list.h"
#include <string.h>
#include <ucontext.h>
typedef struct TELE
{
    const task_const_type *task_bucket;
    task_state_type state;
    task_priority_type cur_priority;
    ucontext_t *cp;
    ucontext_t *main_cp;
    ucontext_t *super_cp;
    int    activation_order;
    struct list_head res_head;
    struct list_head list;
} task_unit_t;

typedef task_unit_t *task_ptr_t, taskItem_t;
extern struct list_head ready_Q, suspend_Q, running_Q;
extern int order;

/* Helper fn*/

/* Insert the task with it priority in descending order */
status_type task_prioinsert(struct list_head*, struct list_head*, task_state_type);


status_type init_queue();
status_type init_iftask2suspend();
status_type init_task2ready();

status_type set_status(task_ptr_t, task_state_type);

status_type activate_task(task_type id);
status_type terminate_task(void);




#endif /* TASK_H */