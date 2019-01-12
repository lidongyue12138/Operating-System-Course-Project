#include "buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <windows.h>
#include <synchapi.h>
#include <time.h>

/* the buffer */
buffer_item buffer[BUFFER_SIZE];
int rear, front;

pthread_mutex_t mutex;
sem_t empty; 
sem_t full; 

int insert_item(buffer_item item);
int remove_item(buffer_item *item);

void *producer(void *param);
void *consumer(void *param);

int insert_item(buffer_item item) {
/* insert item into buffer
return 0 if successful, otherwise
return -1 indicating an error condition */
    if ((rear+1)%BUFFER_SIZE == front)
        return -1;
    buffer[rear] = item;
    rear = (rear+1)%BUFFER_SIZE;
    return 0;
}


int remove_item(buffer_item *item) {
/* remove an object from buffer
placing it in item
return 0 if successful, otherwise
return -1 indicating an error condition */
    if (front == rear)
    {
        return -1;
    }
    item = &buffer[front];
    front = (front+1)%BUFFER_SIZE;
    return 0;
}

void buffer_init()
{
    rear = front = 0;
    pthread_mutex_init(&mutex, NULL);
	sem_init(&full, 0, 0);
	sem_init(&empty, 0, BUFFER_SIZE);
}

void *producer(void *param) {
    buffer_item item;
    while (1) {
    /* sleep for a random period of time */
    sleep(rand()%5);
    /* generate a random number */
    item = rand();

    sem_wait(&empty);
    pthread_mutex_lock(&mutex);

    if (insert_item(item))
        printf("report error condition \n");
    else
        printf("producer produced %d \n",item);

    pthread_mutex_unlock(&mutex);
    sem_post(&full);
    }
}

void *consumer(void *param) {
    buffer_item item;
    while (1) {

    
    /* sleep for a random period of time */
    sleep(rand()%5);

    sem_wait(&full);
    pthread_mutex_lock(&mutex);

    if (remove_item(&item))
        printf("report error condition \n");
    else
        printf("consumer consumed %d \n",item);

    pthread_mutex_unlock(&mutex);
    sem_post(&empty);
    }
}

int main(int argc, char const *argv[])
{
    /* 1. Get command line arguments argv[1],argv[2],argv[3] */
    int sleepTime, producerThreads, consumerThreads;
    if(argc != 4)
	{
		fprintf(stderr, "Useage: <sleep time> <producer threads> <consumer threads>\n");
		return -1;
	}
    sleepTime = atoi(argv[1]);
	producerThreads = atoi(argv[2]);
	consumerThreads = atoi(argv[3]);

    /* 2. Initialize buffer */
    buffer_init();

    /* 3. Create producer thread(s) */
    pthread_t pid[producerThreads], cid[consumerThreads];

	for(int i = 0; i < producerThreads; i++){
		pthread_create(&pid[i],NULL,&producer,NULL);
	}
    
    /* 4. Create consumer thread(s) */

	for(int j = 0; j < consumerThreads; j++){
		pthread_create(&cid[j],NULL,&consumer,NULL);
	}

    /* 5. Sleep */
	sleep(sleepTime);
    
    /* 6. Exit */
    return 0;
}
