#define _GNU_SOURCE
#include <stdio.h>
#include <sched.h>
#include <unistd.h>
#include "sys/types.h"
#include "sys/ipc.h"
#include "sys/shm.h"
#include "string.h"
#include "stdlib.h"
#include <stdbool.h>
struct shared_use_st 
{
	int access_count;
	bool flag;
	
};
struct shared_use_st *shared_stuff;
void create_shared_memory()
{

	int shmid;
	void *shared_memory=(void *)0;
	shmid =shmget( (key_t)1234, 4096, 0666 | IPC_CREAT );
	printf("%d", shmid);
	if (shmid == -1)
	{
		fprintf(stderr,"shmget failed\n");
		exit(EXIT_FAILURE);
	}


	shared_memory =shmat(shmid, (void *)0,0);

	if(shared_memory == (void *)-1)
	{
		fprintf(stderr,"shmat failed\n");
		exit(EXIT_FAILURE);	
	}
   
   	shared_stuff = (struct shared_use_st *)shared_memory;
}
void main ()
{
//printf("\nmy pid = %d  \n",getpid());
cpu_set_t my_set;        
	CPU_ZERO(&my_set);       
	CPU_SET(2, &my_set);     
	sched_setaffinity(0, sizeof(cpu_set_t), &my_set); 
create_shared_memory();
	//shared_stuff->access_count =0;
	shared_stuff->flag =0;
int p = getpid();
  int i, j, k, l;
  int n = 2;
//fork ();
//fork();
FILE *fp;
    fp=fopen("dummy.out","w");    
    fprintf(fp,"dummy %d",p);  
    fclose(fp); 
printf("shared_stuff->flag %d",shared_stuff->flag);
while(!shared_stuff->flag);	

  for (i = 0; i < n; i++)
    {
      
      for (j = 0; j < n; j++)
	{
	  for (k = 0; k < n; k++)
	    {
	      for (l = 0; l < n; l++)
		{
		  int pid = getpid ();
		  printf ("\n i j k l = %d %d %d %d pid = %d my pid = %d", i, j, k, l,
			  pid,p);
		}

	    }
	}
    }
}
