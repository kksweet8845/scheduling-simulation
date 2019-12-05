#ifndef RESOURCE_H
#define RESOURCE_H

#include "typedefine.h"
#include "config.h"
#include "task.h"
#include "list.h"
#include <string.h>
#include <stdio.h>
resource_type resources[RESOURCES_COUNT];
extern struct list_head resource_Q;

typedef struct RELE
{
    resource_type id;
    task_priority_type priority;
    struct list_head list;
} res_ele_t;

typedef res_ele_t* res_ptr_t, resItem_t;



status_type init_resources();
status_type create_resAllo(res_ptr_t*, resource_type, task_priority_type);
status_type get_resource(resource_type id);
status_type release_resource(resource_type id);

#endif /* RESOURCE_H */