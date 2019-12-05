#include "resource.h"



status_type create_resAllo(res_ptr_t *node, resource_type id, task_priority_type prio)
{
    *node = (res_ptr_t) malloc(sizeof(resItem_t));
    if(*node == NULL)
        return STATUS_ERROR;
    (*node)->id = id;
    (*node)->priority = prio;
    INIT_LIST_HEAD(&(*node)->list);
    return STATUS_OK;
}

status_type init_resources()
{
    status_type result = STATUS_OK;
    INIT_LIST_HEAD(&resource_Q);
    for(resource_type i= 0; i < RESOURCES_COUNT; i++)
    {
        resources[i] = 0;
        res_ptr_t newResAllo = NULL;
        result = !result && create_resAllo(&newResAllo, resources_id[i], resources_priority[i]);
        list_add_tail(&newResAllo->list, &resource_Q);
    }
    return result;
}



status_type get_resource(resource_type id)
{
    status_type result = STATUS_OK;
    if(resources[id] == 1 && !list_empty(&running_Q))
    {
        return STATUS_ERROR;
    }
    else if(list_empty(&running_Q))
    {
        printf("Interior error with empty running queue.\n");
        resources[id] = 0;
        return STATUS_ERROR;
    }


    task_ptr_t runningItem = list_first_entry(&running_Q, taskItem_t, list);
    resources[id] = 1;
    if(runningItem->cur_priority < resources_priority[id])
    {
        runningItem->cur_priority = resources_priority[id];
    }
    res_ptr_t item = NULL;
    list_for_each_entry(item, &resource_Q, list)
    {
        if(item->id == id)
        {
            list_del_init(&item->list);
            list_add(&item->list, &runningItem->res_head);
            break;
        }
    }
    return result;
}

status_type release_resource(resource_type id)
{
    status_type result = STATUS_OK;
    if(list_empty(&running_Q))
    {
        resources[id] = 0;
        return STATUS_ERROR;
    }
    task_ptr_t runningItem = list_first_entry(&running_Q, taskItem_t, list);
    res_ptr_t resitem = list_first_entry(&runningItem->res_head, resItem_t, list);
    if(resitem == NULL || resitem->id != id)
    {
        result = STATUS_ERROR;
    }
    else if(resitem->id == id)
    {
        list_del_init(&resitem->list);
        list_add_tail(&resitem->list, &resource_Q);
        resources[id] = 0;
        result = STATUS_OK;
    }
    else
    {
        result = STATUS_ERROR;
    }
    if(!list_empty(&runningItem->res_head))
    {
        resitem = list_first_entry(&runningItem->res_head, resItem_t, list);
        runningItem->cur_priority = resitem->priority;
    }
    else
    {
        runningItem->cur_priority = runningItem->task_bucket->static_priority;
    }
    swapcontext(runningItem->cp, runningItem->main_cp);
    return result;
}