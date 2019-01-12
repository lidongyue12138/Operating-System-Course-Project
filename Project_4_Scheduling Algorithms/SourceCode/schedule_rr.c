#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "schedulers.h"
#include "list.h"
#include "task.h"
#include "cpu.h"

#define SIZE    100
#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))
struct node* head;
int countTask = 0;

// add a task to the list 
void add(char *name, int priority, int burst)
{
    struct task* newTask = malloc(sizeof(struct task));
    newTask->name = name;
    newTask->priority = priority;
    newTask->burst = burst;
    insert(&head, newTask);
    countTask+=1;
}

// invoke the scheduler
void schedule()
{
    struct node* tmp = head;
    while(countTask>0)
    {
        Task *nextTask = tmp->task;

        run(nextTask, 10);
        nextTask->burst -= min(10, nextTask->burst);
        if (nextTask->burst == 0)
        {
            delete(&head, nextTask);
            countTask--;
        }
        
        if (tmp->next == NULL)
            tmp = head;
        else tmp = tmp->next;
    }
}