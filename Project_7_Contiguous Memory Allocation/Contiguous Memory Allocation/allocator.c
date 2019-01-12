#include <stdio.h>  
#include <stdlib.h>  
#include <unistd.h>  
#include <fcntl.h>  
#include <string.h>  
#include <sys/types.h>  
#include <sys/stat.h>  

int memSize;
int simulatedMemory[100];
char * memoryName[100];
int fragNum; // memory fragment number
char emptyName[10] = "Unused"; 

void reportMemory()
{
    int curMem = 0;
    for (int i=0; i<fragNum; i++)
    {
        if (!strcmp(memoryName[i], "Unused"))
        {
            printf("[%d:%d] Unused\n", curMem, curMem+simulatedMemory[i]-1);
        }
        else
        {
            printf("[%d:%d] Process %s\n", curMem, curMem+simulatedMemory[i]-1, memoryName[i]);
        }
        curMem = curMem + simulatedMemory[i];
    }
}

void compactMemory()
{
    int count = 0;
    for (int i=0; i<fragNum; i++)
    {
        if (strcmp(memoryName[i], "Unused"))
        {
            int index = i;
            while(index>0 && !strcmp(memoryName[index-1], "Unused"))
            {
                memoryName[index-1] = memoryName[index];
                int tmp = simulatedMemory[index-1];
                simulatedMemory[index-1] = simulatedMemory[index];

                memoryName[index] = emptyName;
                simulatedMemory[index] = tmp;
                index--;
            }
        }
    }
    for (int i=0; i<fragNum; i++)
    {
        if (!strcmp(memoryName[i], "Unused"))
        {
            int index = i;
            int emptyMemSize = 0;
            while (index < fragNum)
            {
                emptyMemSize += simulatedMemory[index++];
            }
            
            simulatedMemory[i] = emptyMemSize;
            fragNum = i+1;
            break;
        }
    }
}

void allocateMemory(char* name, int size, char* flag)
{
    // printf("name: %s\n", name);
    // printf("size: %d\n", size);
    // printf("flag: %s\n", flag);
    if (flag[0] == 'F')
    {
        int i;
        for (i=0; i<fragNum; i++)
        {
            if (!strcmp(memoryName[i], "Unused"))
            {
                if (size < simulatedMemory[i])
                {
                    for (int j=fragNum; j>i; j--)
                    {
                        memoryName[j] = memoryName[j-1];
                        simulatedMemory[j] = simulatedMemory[j-1]; 
                    }
                    simulatedMemory[i+1] = simulatedMemory[i] - size;
                    simulatedMemory[i] = size;

                    char * tmpName = malloc(sizeof(char)*strlen(name));
                    strcpy(tmpName, name);
                    memoryName[i] = tmpName;
                    fragNum = fragNum + 1;
                    break;
                }
                else if (size == simulatedMemory[i])
                {
                    memoryName[i] = name;
                    break;
                }
            }
        }
        if (i == fragNum) printf("Memory Request Denied.\n");
        return;
    }
    if (flag[0] == 'W')
    {
        int tmpIndex = -1;
        int tmpSize = size-1;
        for (int i=0; i<fragNum; i++)
        {
            if (!strcmp(memoryName[i], "Unused"))
            {
                if (simulatedMemory[i]>tmpSize)
                {
                    tmpIndex = i;
                    tmpSize = simulatedMemory[i];
                }
            }
        }
        if (tmpIndex == -1) 
        {printf("Memory Request Denied.\n"); return;}
        if (size < tmpSize)
        {
            for (int j=fragNum; j>tmpIndex; j--)
            {
                memoryName[j] = memoryName[j-1];
                simulatedMemory[j] = simulatedMemory[j-1]; 
            }
            simulatedMemory[tmpIndex+1] = simulatedMemory[tmpIndex] - size;
            simulatedMemory[tmpIndex] = size;

            char * tmpName = malloc(sizeof(char)*10);
            strcpy(tmpName, name);
            memoryName[tmpIndex] = tmpName;
            fragNum = fragNum + 1;
        }
        else if (size == tmpSize)
        {
            memoryName[tmpIndex] = name;
        }
    }
    if (flag[0] == 'B')
    {
        int tmpIndex = -1;
        int tmpSize = memSize+1;
        for (int i=0; i<fragNum; i++)
        {
            if (!strcmp(memoryName[i], "Unused"))
            {
                if (simulatedMemory[i]>=size && simulatedMemory[i]<tmpSize)
                {
                    tmpIndex = i;
                    tmpSize = simulatedMemory[i];
                }
            }
        }
        if (tmpIndex == -1) 
        {printf("Memory Request Denied.\n"); return;}
        if (size < tmpSize)
        {
            for (int j=fragNum; j>tmpIndex; j--)
            {
                memoryName[j] = memoryName[j-1];
                simulatedMemory[j] = simulatedMemory[j-1]; 
            }
            simulatedMemory[tmpIndex+1] = simulatedMemory[tmpIndex] - size;
            simulatedMemory[tmpIndex] = size;

            char * tmpName = malloc(sizeof(char)*10);
            strcpy(tmpName, name);
            memoryName[tmpIndex] = tmpName;
            fragNum = fragNum + 1;
        }
        else if (size == tmpSize)
        {
            memoryName[tmpIndex] = name;
        }
    }
}

void releaseMemory(char* name)
{
    for (int i=0; i<fragNum; i++)
    {
        if (memoryName[i][1] == name[1])
        {
            memoryName[i] = emptyName;
            if (i>0 && !strcmp(memoryName[i-1], "Unused"))
            {
                simulatedMemory[i-1] += simulatedMemory[i];
                for (int j=i; j<fragNum-1; j++)
                {
                    simulatedMemory[j] = simulatedMemory[j+1];
                    memoryName[j] = memoryName[j+1];
                }
                fragNum--;
                i--;
            }
            if (i<fragNum-1 && !strcmp(memoryName[i+1], "Unused"))
            {
                simulatedMemory[i] += simulatedMemory[i+1];
                for (int j=i+1; j<fragNum-1; j++)
                {
                    simulatedMemory[j] = simulatedMemory[j+1];
                    memoryName[j] = memoryName[j+1];
                }
                fragNum--;
            }
            break;
        }
    }
}

int main(int argc, char *argv[])
{
    memSize = atoi(argv[1]);;

    for (int i=0; i<100; i++) simulatedMemory[i] = 0;
    fragNum = 1;
    simulatedMemory[0] = memSize;
    memoryName[0] = emptyName;

    int status = 1;

	/*Excute input lines*/
	while (status)
	{
        char* cmd = malloc(sizeof(char)*30);
        printf("allocator>");
        fgets(cmd, 30, stdin);

        char * pch;
        pch = strtok (cmd," ");

		if(cmd[0] == 'C')
		{
			compactMemory();
		}
		else if (cmd[1] == 'Q')
		{
            char* name;
            int size;
            char* flag;

            name = strtok(NULL, " ");
            size = atoi(strtok(NULL, " "));
            flag = strtok(NULL, " ");

			allocateMemory(name, size, flag);
		}
		else if (cmd[1] == 'L')
		{
			char* name;
            name = strtok(NULL, " ");
            releaseMemory(name);
        }
		else if(cmd[0] == 'S')
		{
		    reportMemory();
		}
        else if(cmd[0] == 'X')
        {
            status = 0;
        }
	}
    return 0;
}
