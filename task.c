#include "task.h"





/* Helper section */
status_type task_prioinsert(struct list_head* node,
                            struct list_head* head,
                            task_state_type type)
{
    task_ptr_t item = NULL;
    task_ptr_t cur = list_entry(node, taskItem_t, list);
    cur->state = type;
    task_priority_type cur_priority = cur->cur_priority;

    if(list_empty(head))
    {
        list_add(node, head);
        return STATUS_OK;
    }
    list_for_each_entry(item, head, list)
    {
        task_priority_type item_priority  = item->cur_priority;
        if( item_priority < cur_priority)
        {
            list_insert_before(&item->list, node);
            return STATUS_OK;
        }
        else if( item_priority == cur_priority )
        {
            if(item->activation_order > cur->activation_order)
            {
                list_insert_before(&item->list, node);
            }
            else
                list_insert_after(&item->list, node);
            return STATUS_OK;
        }
        else if(item->list.next == head)
        {
            list_insert_after(&item->list, node);
            return STATUS_OK;
        }
    }
    return STATUS_ERROR;
}

/* =====================================================================================  */


status_type init_queue()
{
    INIT_LIST_HEAD(&ready_Q);
    INIT_LIST_HEAD(&suspend_Q);
    INIT_LIST_HEAD(&running_Q);

    return STATUS_OK;
}

status_type init_task2suspend()
{
    for(int i=0; i<TASKS_COUNT; i++)
    {
        task_ptr_t newTask = (task_ptr_t) malloc(sizeof(taskItem_t));
        newTask->task_bucket = &task_const[i];
        newTask->state = SUSPENDED;
        newTask->cur_priority = newTask->task_bucket->static_priority;
        newTask->activation_order = -1;
        INIT_LIST_HEAD(&newTask->res_head);
        INIT_LIST_HEAD(&newTask->list);
        task_prioinsert(&newTask->list, &suspend_Q, SUSPENDED);
    }

    return STATUS_OK;
}



status_type init_task2ready()
{

    task_ptr_t cur;
    for(task_type i=0; i<AUTO_START_TASKS_COUNT; i++)
    {
        task_ptr_t item = NULL;
        list_for_each_entry(item, &suspend_Q, list)
        {
            if(item->task_bucket->id == auto_start_tasks_list[i])
            {
                cur = item;
                list_del_init(&item->list);
                item->activation_order = order++;
                task_prioinsert(&cur->list, &ready_Q, READY);
                break;
            }
        }
    }

    return STATUS_OK;
}




status_type activate_task(task_type id)
{
    status_type result = STATUS_OK;
    task_ptr_t item = NULL;
    task_ptr_t cur = NULL;
    list_for_each_entry(item, &suspend_Q, list)
    {
        if(item->task_bucket->id == id)
        {
            list_del_init(&item->list);
            cur = item;
            makecontext(cur->cp, cur->task_bucket->entry, 0);
            break;
        }
    }
    if(cur == NULL)
    {
        result = STATUS_ERROR;
        return result;
    }
    cur->activation_order = order++;
    task_prioinsert(&cur->list, &ready_Q, READY);
    task_ptr_t runningItem = list_first_entry(&running_Q, taskItem_t, list);
    swapcontext(runningItem->cp, runningItem->main_cp);
    // list_add(&runningItem->list, &running_Q);
    return result;
}

status_type terminate_task(void)
{
    status_type result = STATUS_OK;

    task_ptr_t runningItem = list_first_entry(&running_Q, taskItem_t, list);
    if(!list_empty(&runningItem->res_head))
    {
        return STATUS_ERROR;
    }
    list_del_init(&runningItem->list);
    task_prioinsert(&runningItem->list, &suspend_Q, SUSPENDED);
    setcontext(runningItem->main_cp);
    return result;
}