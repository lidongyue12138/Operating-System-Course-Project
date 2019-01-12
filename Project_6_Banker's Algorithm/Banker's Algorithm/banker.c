#include <stdio.h>  
#include <stdlib.h>  
#include <unistd.h>  
#include <fcntl.h>  
#include <string.h>  
#include <sys/types.h>  
#include <sys/stat.h>  

#define FILE_NAME "./max.txt"
#define NUMBER_OF_CUSTOMERS 5 
#define NUMBER_OF_RESOURCES 4

int available[NUMBER_OF_RESOURCES];
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
int flags[NUMBER_OF_CUSTOMERS];

void release_resources(int customer_num, int release[])
{
	int i;
	for(i = 0; i < NUMBER_OF_RESOURCES; i++)
	{
		if(release[i] > allocation[customer_num][i])
		{
			printf("RELEASE EXCEED ALLOCATION\n");
			return;
		}
	}
	for(i = 0; i < NUMBER_OF_RESOURCES; i++)
	{
		allocation[customer_num][i] -= release[i];
		available[i] += release[i];
	}
	update();
}

void update()
{
	for(int i = 0; i < NUMBER_OF_CUSTOMERS; i++)
	{
		for(int j = 0; j < NUMBER_OF_RESOURCES; j++)
		{
			need[i][j] = maximum[i][j] - allocation[i][j];
		}
	}
}

int request_resources(int customer_num, int request[])
{
	for(int i = 0; i < NUMBER_OF_RESOURCES; i++)
	{
		if(request[i] > maximum[customer_num][i])
		{
			printf("REQUEST EXCEED MAXIMUM\n");
			return 0;
		}
	}
	for(int i = 0; i < NUMBER_OF_RESOURCES; i++)
	{
		allocation[customer_num][i] = request[i];
	}
	update();
	
	int count = 0, copy[NUMBER_OF_RESOURCES];
	for(int i = 0; i < NUMBER_OF_CUSTOMERS; i++){
		flags[i] = 1;
	}
	for(int i = 0; i < NUMBER_OF_RESOURCES; i++){	
		copy[i] = available[i];
	}
	while(1){
		for(int i = 0; i < NUMBER_OF_CUSTOMERS; i++){
			if(flags[i]){
				int j;
				for(j = 0; j < NUMBER_OF_RESOURCES; j++){
					if(available[j] < need[i][j])
						break;
				}
				if(j == NUMBER_OF_RESOURCES){
					flags[i] = 0;
					count++;
					for(int j = 0; j < NUMBER_OF_RESOURCES; j++)
						available[j] += allocation[i][j];
				}
			}
		}
		if(count == NUMBER_OF_CUSTOMERS){
			for(int i = 0; i < NUMBER_OF_RESOURCES; i++)
				available[i] = copy[i];
			return 1;
		}
		if(count == 0)
		{
			for(int i = 0; i < NUMBER_OF_RESOURCES; i++)
				available[i] = copy[i];
			return 0;
		}
	}
}

void readFile()
{
    FILE * file_fd;  
    int read_len, i, cid = 0, j = 0, rid = 0;  
    char file_buffer[2 * NUMBER_OF_RESOURCES], char_data[5];
  
    file_fd = fopen(FILE_NAME,"rb");  
    if(!file_fd)  
    {  
       perror("open file error\n");
       exit(0); 
    } 
  
    while(1)  
    {  
       read_len = fread(file_buffer, 1, 2 * NUMBER_OF_RESOURCES, file_fd);  
         
       if(read_len == -1)  
       {  
           printf("File read error!\n");  
           perror("errno");  
           exit(0);  
       }  
       else if(read_len == 0)  
       {    
           break;  
       }  
       else   
       {    
       	   rid = 0;
           for(i = 0; i < read_len; i++)  
           {  
                if(file_buffer[i] == ',' || file_buffer[i] == '\n')
                {
                	char_data[j] = '\0';
                	j = 0;
                	maximum[cid][rid] = atoi(char_data);
                	rid++;
                }
                else
                {
                	char_data[j] = file_buffer[i];
                	j++;
                }
           }

       }
       cid++;    
    }  
    fclose(file_fd);    
}



void display()
{
	int i, j;

	printf("Avaliable\n");
	for(i = 0; i < NUMBER_OF_RESOURCES; i++)
	{
		printf("%d\t", available[i]);
	}
	printf("\n");

	printf("Maximum\n");
	for(i = 0 ; i < NUMBER_OF_CUSTOMERS; i++)
	{
		printf("T%d\t", i);
		for(j = 0; j < NUMBER_OF_RESOURCES; j++)
		{
			printf("%d\t", maximum[i][j]);
		}
		printf("\n");
	}

	printf("Allocation\n");
	for(i = 0 ; i < NUMBER_OF_CUSTOMERS; i++)
	{
		printf("T%d\t", i);
		for(j = 0; j < NUMBER_OF_RESOURCES; j++)
		{
			printf("%d\t", allocation[i][j]);
		}
		printf("\n");
	}

	printf("Need\n");
	for(i = 0 ; i < NUMBER_OF_CUSTOMERS; i++)
	{
		printf("T%d\t", i);
		for(j = 0; j < NUMBER_OF_RESOURCES; j++)
		{
			printf("%d\t", need[i][j]);
		}
		printf("\n");
	}
}

void normalize(char *cmd, int *target_thread, int args[])
{
	int i = 3, j = 0, k = 0, flag = 1;
	char tmp[5];
	while(1)
	{
		if(cmd[i] == ' ')
		{
			tmp[j] = '\0';
			j = 0;
			if(flag)
			{
				flag = 0;
				*target_thread = atoi(tmp);
			}
			else
			{
				args[k] = atoi(tmp);
				k++;
			}
		}
		else if(cmd[i] == '\n')
		{
			args[k] = atoi(tmp);break;
		}
		else
		{
			tmp[j++] = cmd[i];
		}
		i++;
	}
}

int main(int argc, char *argv[])
{
	
	int args[NUMBER_OF_RESOURCES], target_thread; 

	if(argc != NUMBER_OF_RESOURCES + 1)
	{
		fprintf(stderr, "Useage: %d is required.\n", NUMBER_OF_RESOURCES);
		return -1;
	}
	for(int i = 1; i < argc; i++)
	{
		available[i - 1] = atoi(argv[i]);
	}

	readFile();
	update();
	char cmd[20];

	/*Excute input lines*/
	while (fgets(cmd, 20, stdin))
	{
		if(cmd[0] == '*')
		{
			display();
		}
		else if (cmd[1] == 'Q')
		{
			normalize(cmd, &target_thread, args);
			if(request_resources(target_thread, args))
			{
				printf("REQUEST COMMAND SAFE\n");
			}
			else
			{
				printf("REQUEST COMMAND UNSAFE\n");
			}
		}
		else if (cmd[1] == 'L')
		{
			normalize(cmd, &target_thread, args);
			release_resources(target_thread, args);
		}
		else if(cmd[0] == '!')
		{
			break;
		}
	}
}
