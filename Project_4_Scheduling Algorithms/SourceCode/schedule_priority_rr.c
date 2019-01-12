#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "schedulers.h"
#include "list.h"
#include "task.h"
#include "cpu.h"

#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))

#define SIZE    100
struct node* headList[11] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
int countTask = 0;

// add a task to the list 
void add(char *name, int priority, int burst)
{
    struct task* newTask = malloc(sizeof(struct task));
    newTask->name = name;
    newTask->priority = priority;
    newTask->burst = burst;
    insert(&headList[priority], newTask);
    countTask+=1;
}

// invoke the scheduler
void schedule()
{
    int curPriority = 10;
    struct node* tmp = headList[curPriority];
    while(countTask > 0)
    {
        while (tmp == NULL)
        {
            tmp = headList[--curPriority];
        }
        Task *nextTask = tmp->task;
        

        run(nextTask, 10);
        nextTask->burst -= min(10, nextTask->burst);
        if (nextTask->burst == 0)
        {
            delete(&headList[curPriority], nextTask);
            countTask--;
        }

        if (tmp->next == NULL)
            tmp = headList[curPriority];
        else tmp = tmp->next; 
    }
}