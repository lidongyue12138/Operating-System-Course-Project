#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

int *array; int * resArray;
int n;
int count = 0;
const int sortingThreadNum = 2;

void sort(int low, int high)
{
    for (int i=low; i<high; i++)
    {
        int tmp = i;
        for (int j=i+1; j<high+1; j++)
        {
            if (array[j] < array[tmp])
                tmp = j;
        }
        int tmpVal = array[tmp];
        array[tmp] = array[i];
        array[i] = tmpVal;
    }
}

void *sortAlgorithm(void *arg)
{
    int curThread = count++;

    int low = curThread*(n/sortingThreadNum);
    int high = (curThread+1)*(n/sortingThreadNum) - 1;
    sort(low, high);
}

void *merge(void *arg)
{
    int index1 = 0, index2 = n/sortingThreadNum;
    for (int i=0; i<n; i++)
    {
        if ( index1 < n/sortingThreadNum && index2 < n)
        {
            if (array[index1] < array[index2]) resArray[i] = array[index1++];
            else resArray[i] = array[index2++];
        }
        else
        {
            if (index1 < n/sortingThreadNum) resArray[i] = array[index1++];
            if (index2 < n) resArray[i] = array[index2++];
        }
    }
}

int main()
{
    scanf("%d", &n);
    array = new int[n]; resArray = new int[n];
    for (int i=0; i<n; i++)
    {
        scanf("%d", &array[i]);
    }

    pthread_t sortingThreads[sortingThreadNum];
    for (int i=0; i<sortingThreadNum; i++)
    {
        pthread_create(&sortingThreads[i], NULL, sortAlgorithm, (void*)NULL);
    }

    for (int i = 0; i < sortingThreadNum; i++) 
        pthread_join(sortingThreads[i], NULL); 

    for (int i=0; i<n; i++) printf("%d ", array[i]);
    printf("\n"); 

    pthread_t mergingThread;
    pthread_create(&mergingThread, NULL, merge, NULL);
    pthread_join(mergingThread, NULL);

    for (int i=0; i<n; i++) printf("%d ", resArray[i]);
    printf("\n"); 

    delete [] array; delete [] resArray;
}