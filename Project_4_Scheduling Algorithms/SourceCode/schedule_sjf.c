#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "schedulers.h"
#include "list.h"
#include "task.h"
#include "cpu.h"

#define SIZE    100
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
    while(countTask>0)
    {
        Task *ShortestTask = head->task;
        
        struct node *temp;
        temp = head;

        while (temp != NULL) {
            if (temp->task->burst < ShortestTask->burst)
                ShortestTask = temp->task;
            temp = temp->next;
        }

        run(ShortestTask, ShortestTask->burst);
        delete(&head, ShortestTask);
        countTask--;
    }
}