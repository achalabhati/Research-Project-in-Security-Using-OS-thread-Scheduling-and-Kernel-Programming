/* In this program, NTHREADS no of threads are created in parent process (i.e. T1,T2,T3,...). Initially threads T2,T3,.... (except T1 thread) and child process 
are in waiting/blocking state.

Child process is waiting for signal from 1st thread T1. T1 goes into blocking state 
after sending signal to child process and remains in blocking state until it receive signal from child process.

Then child process sends signal to next thread T2 and wait for singal from next thread //achala
 
*/

#define _GNU_SOURCE             /* See feature_test_macros(7) */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <pthread.h>
#include <sched.h>
#include <syscall.h>
#include "stdbool.h"

#ifndef _POSIX_THREAD_PROCESS_SHARED
#error This system does not support process shared mutex
#endif

#define NUM_THREADS  25
#define MAX_COUNT 5000


int segmentId;
int segmentChildId;
int	segmentcheckId;
int	segmentFinishId;
int *currThread;
int *child;
int *Finish;
unsigned long long int tic1,tic2;
pthread_cond_t *cvptr[NUM_THREADS+1];    //Condition Variable Pointers of parent threads
pthread_condattr_t cattr[NUM_THREADS+1]; //Condition Variable Attributes of parent threads

pthread_cond_t *cvptrChild;    //Condition Variable Pointers of Child
pthread_condattr_t cattrChild; //Condition Variable Attributes of Child


pthread_mutex_t    *mptr[NUM_THREADS+1]; //Mutex Pointers
pthread_mutexattr_t matr[NUM_THREADS+1]; //Mutex Attributes

pthread_mutex_t    *mptrChild; //Mutex Pointers
pthread_mutexattr_t matrChild; //Mutex Attributes


int   shared_mem_id;      //shared memory Id
int   *mp_shared_mem_ptr; //shared memory ptr -- pointing to mutex
int   *cv_shared_mem_ptr; //shared memory ptr -- pointing to condition variable

inline void clflush(volatile void *p)
{
    asm volatile ("clflush (%0)" :: "r"(p));
}

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


// Thread info
struct info {
    long id;
    int spy_loop;
};
 
void *parentThreads(void *data)
{
	struct info *info = data;
   	long tid;
	tid = info->id;
	int SPY_LOOP=info->spy_loop;
	
	int sid = syscall(SYS_gettid);
	//printf("thread id=%d\n",sid);
	
	int counter = 0;
	unsigned long long start,end;
	//char changed_shared_variable_access_count[MAX_COUNT/NUM_THREADS];
	//unsigned long long changed_shared_variable_check_count[MAX_COUNT];
	
	int k=0;
	FILE *fp2,*fp3;
   
	pthread_t thread;
	thread = pthread_self();
		
	cpu_set_t my_set;        
	CPU_ZERO(&my_set);       
	CPU_SET(2, &my_set);     
	sched_setaffinity(0, sizeof(cpu_set_t), &my_set); //achala
    int s;
  
 	

				
				
			
	while(1)
	{
		
		pthread_mutex_lock( mptr[tid]);
		while (*currThread  != tid)
		{
		 	pthread_cond_wait(cvptr[tid], mptr[tid]);

		}
		pthread_mutex_unlock(mptr[tid]);
  
		// 	store value of shared variable at the time of receiving signal
		
			
		if( tid ==0){ // notify V to start AES. We delayed it as to start AES at stable condition //achala
			printf("\n flag settttt\n");			
			shared_stuff->flag =1;
 			
		}

	   	
			pthread_mutex_lock( mptrChild );	
			*currThread = (*currThread + 1)%NUM_THREADS;
			*child=1;
			pthread_mutex_unlock( mptrChild );	
		
		}		 
		counter++;	
		volatile int check_counter=0;
		while(check_counter++<SPY_LOOP);
		 
		if(counter==MAX_COUNT/NUM_THREADS)	
		{
			break;
		}
	}


			if(tid == NUM_THREADS-1)
			{
				printf("End of operation\n");
				*Finish =1; // Denote end of operation 
			}	
			pthread_mutex_unlock(mptrChild );
			
	pthread_exit(NULL);
}



int main(int argc, char *argv[])
{
	int  rtn;
	size_t shm_size;

	/* initialize shared memory segment */
	shm_size = (NUM_THREADS+1)*sizeof(pthread_mutex_t) + (NUM_THREADS+1)*sizeof(pthread_cond_t); // for new cond_variable we added 1*sizeof(pthread_cond_t)

	if ((shared_mem_id = shmget(IPC_PRIVATE, shm_size, 0660)) < 0)
	{
        perror("shmget"), exit(1) ;
	}
	if ((mp_shared_mem_ptr = (int *)shmat(shared_mem_id, (void *)0, 0)) == NULL)
	{
        perror("shmat"), exit(1);
	}

	//	 Offset to find the location of the condition variable in the shared memory
	//   unsigned char* byte_ptr = reinterpret_cast<unsigned char*>(mp_shared_mem_ptr);
	
	int nt;
	unsigned char* byte_ptr = (unsigned char*) (mp_shared_mem_ptr);    
	for(nt = 0; nt<=NUM_THREADS; nt++){
	    mptr[nt] = (pthread_mutex_t *)byte_ptr;	    
	    byte_ptr += 1*sizeof(pthread_mutex_t);
	    cvptr[nt] = (pthread_cond_t *)byte_ptr;
	    byte_ptr += 1*sizeof(pthread_cond_t);
	}	    
	mptrChild = (pthread_mutex_t *)mptr[NUM_THREADS];	    
	cvptrChild = (pthread_cond_t *)cvptr[NUM_THREADS];
  
    // Setup Mutex
	for(nt = 0; nt<=NUM_THREADS; nt++)
	{
		if (rtn = pthread_mutexattr_init(&matr[nt])) 
	    	{
	       		 fprintf(stderr,"pthreas_mutexattr_init: %s",strerror(rtn)),exit(1);
	    	}
	    	if (rtn = pthread_mutexattr_setpshared(&matr[nt],PTHREAD_PROCESS_SHARED))
	    	{
			fprintf(stderr,"pthread_mutexattr_setpshared %s",strerror(rtn)),exit(1);
	    	}
	    	if (rtn = pthread_mutex_init(mptr[nt], &matr[nt]))
	    	{
	       	 	fprintf(stderr,"pthread_mutex_init %s",strerror(rtn)), exit(1);
	    	}

	   	//Setup Condition Variable
	   	if(rtn = pthread_condattr_init(&cattr[nt]))
	    	{
			fprintf(stderr,"pthread_condattr_init: %s",strerror(rtn)),exit(1);
	    	}
	   	if(pthread_condattr_setpshared(&cattr[nt], PTHREAD_PROCESS_SHARED )) 
	    	{
			fprintf(stderr,"pthread_condattr_setpshared %s",strerror(rtn)),exit(1);
	    	}
	    	if(pthread_cond_init(cvptr[nt], &cattr[nt]))
	    	{
	       		 fprintf(stderr,"pthread_cond_init %s",strerror(rtn)),exit(1);
	    	}
    	}

	int shareSize = sizeof(int) * (2 + 1);
	segmentId = shmget(IPC_PRIVATE, shareSize, 0660);  
	currThread = (int *) shmat(segmentId, NULL, 0);  
	*currThread = 0; // shared variable for thread among parent and child
	
	int sharechild = sizeof(int) * (2 + 1);
	segmentChildId = shmget(IPC_PRIVATE, sharechild, 0660); 
	child = (int *) shmat(segmentChildId, NULL, 0);  
	*child = 0; // shared variable for thread among parent and child
 
 	int shareFinish = sizeof(int) * (2 + 1);
	segmentFinishId = shmget(IPC_PRIVATE, shareFinish , 0660); 
	Finish = (int *) shmat(segmentFinishId, NULL, 0);  
	*Finish = 0; // denote end of operation
 
 
	create_shared_memory();
	 
	pid_t pid, id;
	int i;

	pid = fork();
	id =  getpid();

	if(pid > 0)
	{
		//In parent
		pthread_t threads[NUM_THREADS];
		int rc;
		long t;
		for(t=0;t<NUM_THREADS;t++)
		{
			struct info *info = malloc(sizeof(struct info));
			info->spy_loop=(atoi)(argv[1]);
			info->id=t; // thread id
			
			rc = pthread_create(&threads[t], NULL, parentThreads, (void *)info);
			if (rc)
			{
		       		printf("ERROR; return code from pthread_create() is %d\n", rc);
		       		exit(-1);
		       	}
		}
		pthread_exit(NULL);
  
	}
	else
	{
		// In Child

		pthread_t thread;
		thread = pthread_self();	
		cpu_set_t my_set; /* Define your cpu_set bit mask. */
		CPU_ZERO(&my_set); /* Initialize it all to 0, i.e. no CPUs selected. */         //achala
		CPU_SET(1, &my_set);/* set the bit that represents core 1. */
		sched_setaffinity(0, sizeof(cpu_set_t), &my_set);/*Set affinity of this process*/
	 
		int SPY_MASTER_LOOP=(atoi)(argv[2]);
		int check_counter=0;
		long int counter = 0;
		while(counter++<MAX_COUNT)
		{						
			if(*Finish == 1) break;	
			
			while(!*child){
				if(*Finish == 1) break;	
			};
			
			volatile int wait_child=0;
			while(wait_child++<SPY_MASTER_LOOP);
			
			pthread_cond_signal( cvptr[*currThread] );
			pthread_mutex_lock( mptrChild );
			*child=0;
			pthread_mutex_unlock( mptrChild );			
			
			// resend signal to make sure next thread is awaken
			pthread_cond_signal( cvptr[*currThread] );
			pthread_cond_signal( cvptr[*currThread] );
			pthread_cond_signal( cvptr[*currThread] );		  	
		 	
		}
		
	}

return 1;
}

