#include "resource.h"
#include "task.h"
#include "list.h"

#include <stdbool.h>
#include <ucontext.h>
#include <errno.h>

#ifdef _LP64
#define STACK_SIZE 2097152+16384
#else
#define STACK_SIZE 16384
#endif

int errno;
struct list_head ready_Q, suspend_Q, running_Q;
struct list_head resource_Q;
ucontext_t main_context;
int order = 0;

bool init_context()
{
    bool ok = true;
    task_ptr_t item = NULL;
    list_for_each_entry(item, &suspend_Q, list)
    {
        item->cp = (ucontext_t*) malloc(sizeof(ucontext_t));
        ok = ok && !getcontext(item->cp);
        item->cp->uc_link = 0;
        if( (item->cp->uc_stack.ss_sp = (char*) malloc(STACK_SIZE)) != NULL)
        {
            item->cp->uc_stack.ss_size = STACK_SIZE;
            item->cp->uc_stack.ss_flags = 0;
            errno = 0;
            makecontext(item->cp, item->task_bucket->entry, 0);
            if(errno != 0)
            {
                printf("%d\n", errno);
                return false;
            }
        }
    }
    return ok;
}

bool simulate()
{
    while(1)
    {
        getcontext(&main_context);
        task_ptr_t item = list_first_entry(&ready_Q, taskItem_t, list);
        task_ptr_t temp = NULL;
        // list_for_each_entry(temp, &ready_Q, list){
        //     printf("%d %d\n", temp->task_bucket->id, temp->cur_priority );
        // }
        if(list_empty(&running_Q))
        {
            item->main_cp = &main_context;
            list_del_init(&item->list);
            list_add(&item->list, &running_Q);
            setcontext(item->cp);
            continue;
        }
        task_ptr_t runningItem = list_first_entry(&running_Q, taskItem_t, list);
        if(runningItem->cur_priority < item->cur_priority)
        {
            list_del_init(&runningItem->list);
            task_prioinsert(&runningItem->list, &ready_Q, READY);
            list_del_init(&item->list);
            task_prioinsert(&item->list, &running_Q, RUNNING);
            item->main_cp = &main_context;
            item->super_cp = runningItem->cp;
            setcontext(item->cp);
        }
        else
        {
            setcontext(runningItem->cp);
        }
    }
}

int main()
{
    bool ok = true;


    ok = ok && !init_queue();
    ok = ok && !init_task2suspend();
    ok = ok && init_context();
    ok = ok && !init_task2ready();
    ok = ok && !init_resources();
    ok = ok && simulate();

    return 0;
}