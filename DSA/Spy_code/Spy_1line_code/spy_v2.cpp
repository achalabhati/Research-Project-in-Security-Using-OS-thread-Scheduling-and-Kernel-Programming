//#define _GNU_SOURCE             /* See feature_test_macros(7) */

//#define _GNU_SOURCE             /* See feature_test_macros(7) */
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sched.h>
#include <syscall.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdbool.h>
#include <semaphore.h>

#include <stdint.h>
#include <time.h>
#include <time.h>
#include <sys/time.h>
#include <syscall.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "bn_lcl.h"
#include <openssl/dsa.h>
#include <openssl/err.h>
#include <openssl/sha.h>
#include <openssl/bn.h>
#include <fstream>
#include <iostream>
#include <sstream>

#ifndef _POSIX_THREAD_PROCESS_SHARED
#error This system does not support process shared mutex
#endif

using namespace std;

#define NUM_THREADS 250 
/*MAX_COUNT should be multiple of NUM_THREADS */
#define MAX_COUNT 50000


void bn_mul_recursive(BN_ULONG *r, BN_ULONG *a, BN_ULONG *b, int n2,int dna, int dnb, BN_ULONG *t);
void bn_sqr_recursive(BN_ULONG *r, const BN_ULONG *a, int n2, BN_ULONG *t);
void bn_sqr_comba8(BN_ULONG *r, const BN_ULONG *a);
int segmentId;
int segmentId2;
int segmentChildId;
int segmentChildId2;


int	segmentFinishId;
int *currThread;
int *child;
int *child2;
int *Finish;
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


// Thread info
struct Info {
    long id;
    int spy_loop;
};
int glob_counter=0; 

void *parentThreads(void *data)
{
	
	struct Info *info =(struct Info *)data;
   	long tid;
	tid = info->id;
	int SPY_LOOP=info->spy_loop;
	 
	int changed_shared_variable_access_count[MAX_COUNT/NUM_THREADS];
		
	cpu_set_t my_set;        
	CPU_ZERO(&my_set);       
	CPU_SET(2, &my_set);     
	sched_setaffinity(0, sizeof(cpu_set_t), &my_set); 
    
    
    int sid = syscall(SYS_gettid);
    FILE *fp;
    fp=fopen("ipc.out","a");    
    fprintf(fp,"thread_%ld %d\n",tid,sid);  
    fclose(fp); 

  /*
   if(tid==0){
	printf("Address BN_MUL %p\n",&BN_mul);
	printf("Address BN_SQR %p\n",&BN_sqr);	
	}*/
		
		

		int ii;
		long int counter = 0;
	    unsigned long proberesult_s,proberesult_m;
		char *s,*m;
	
		s = ((char *)(&BN_sqr));
		m = ((char *)(&BN_mul));
		
		bool values1[MAX_COUNT/NUM_THREADS]={0};
		bool values2[MAX_COUNT/NUM_THREADS]={0};
		
		for(ii=0;ii<MAX_COUNT/NUM_THREADS;ii++)
		{
			values1[ii]=0;
			values2[ii]=0;			
		}
		
		int sum=0;	 
		// Bringing into cache
		sum+=(*s);
		sum+=(*m);
		
		// Flush from cache
		clflush((void *)s);
		clflush((void *)m);

	while(1)
	{
		pthread_mutex_lock( mptr[tid]);
		while (*currThread  != tid)
		{
		 	pthread_cond_wait(cvptr[tid], mptr[tid]);
		}
		pthread_mutex_unlock(mptr[tid]);
 			//printf("Awaken %ld\n",tid);
 
		if(counter==30 && tid ==0){ // notify V to start AES. We delayed it as to start AES at stable condition 
			*start_flag=1;
		//printf("Now start DSA Signature, *access_count=%d\n",*access_count);
		}	
			
		
	   	if (*currThread  == tid)
		{
			// 	store value of shared variable at the time of receiving signal
			changed_shared_variable_access_count[counter]=*access_count;
			// Do the measurement ; Find the access time 
			proberesult_s = probe(s);					 
			proberesult_m = probe(m);	

			values1[counter] = proberesult_s;
			values2[counter] = proberesult_m;
		  					
			// signal to child 
			pthread_mutex_lock( mptrChild );	
			*currThread = (*currThread + 1)%NUM_THREADS;
			*child=1;
			pthread_mutex_unlock( mptrChild );		
		}		
		
		struct timespec t1,t2;
		clock_gettime(CLOCK_MONOTONIC_COARSE,&t1);
		while(*child2==0){ // wait for notification, if notification doesn't received till 200nsec, current spy will send signal to next spy
			clock_gettime(CLOCK_MONOTONIC_COARSE,&t2);
			
			if(t2.tv_nsec-t1.tv_nsec >1000)
			{
				//printf("Spy %ld, sending signal \n",tid);
				pthread_cond_signal( cvptr[*currThread] );
				break;
			}
			
		}
		counter++;
		 
		if(counter==MAX_COUNT/NUM_THREADS)	
		{
		//	printf("Finishing %ld\n",tid);	
			break;
		}
	}

			
			FILE *fp1,*fp2;		
			pthread_mutex_lock( mptrChild );
		
			fp1=fopen("probe1.txt","a");
			fp2=fopen("probe2.txt","a");

			
			for(counter=0;counter<MAX_COUNT/NUM_THREADS;counter++)
			{
				fprintf(fp2,"%5ld,%5d,%5ld,%5d,%5d\n",counter,tid,changed_shared_variable_access_count[counter],values1[counter],values2[counter]);
				if( values1[counter] || values2[counter])
					fprintf(fp1,"%5ld,%5d,%5ld,%5d,%5d\n",counter,tid,changed_shared_variable_access_count[counter],values1[counter],values2[counter]);
			}
			fclose(fp1);
			fclose(fp2);
			
			//fclose(fp3);
			if(tid == NUM_THREADS-1)
			{
				//printf("End of operation\n");	
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

	int sharechild2 = sizeof(int) * (2 + 1);
	segmentChildId2 = shmget(IPC_PRIVATE, sharechild2, 0660); 
	child2 = (int *) shmat(segmentChildId2, NULL, 0);  
	*child2 = 0; // shared variable for thread among parent and child



	int shareFinish = sizeof(int) * (2 + 1);
	segmentFinishId = shmget(IPC_PRIVATE, shareFinish , 0660); 
	Finish = (int *) shmat(segmentFinishId, NULL, 0);  
	*Finish = 0; // denote end of operation
 
 
 
	create_shared_memory();
	pid_t pid;

	pid = fork();
	if(pid > 0)
	{
		//sleep(1); // make sure Spy_Master gets ready
		//In parent
		pthread_t threads[NUM_THREADS];
		
		
	
	
		int rc;
		long t;
		for(t=0;t<NUM_THREADS;t++)
		{
			struct Info *info = (struct Info *)malloc(sizeof(struct Info));
			info->spy_loop=(atoi)(argv[1]);
			info->id=t; // thread id
			
			//printf("Creating threads \n");
			rc = pthread_create(&threads[t], NULL, parentThreads, (void *)info);
			if (rc)
			{
				printf("Error,,, info->id =%ld\n",info->id);
			
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
		CPU_ZERO(&my_set); /* Initialize it all to 0, i.e. no CPUs selected. */
		CPU_SET(1, &my_set);/* set the bit that represents core 1. */
		sched_setaffinity(0, sizeof(cpu_set_t), &my_set);/*Set affinity of this process*/
	 
		int SPY_MASTER_LOOP=(atoi)(argv[2]);
		int check_counter=0;
		long int counter = 0;
		volatile int num=0,s;
		while(counter++<MAX_COUNT)
		{	
								
			if(*Finish == 1) break;			
			
			// Waiting for notification of completion from SPY so that it can awaken next SPY thread
			while( !*child ){
				if(*Finish == 1) break;					
			}; // wait till set by Thread

			*child2=1; // notify Spy to release CPU 
			
			volatile int wait_child=0;			
			while(wait_child++<SPY_MASTER_LOOP);
			
			
			pthread_cond_signal( cvptr[*currThread] ); // sends signal to next spy thread
			pthread_cond_signal( cvptr[*currThread] );					  	
			//printf("Sending signal to %d\n",*currThread);
			
			pthread_mutex_lock( mptrChild );
			*child=0;	
			*child2=0;	//reset	 
			pthread_mutex_unlock( mptrChild );
		 	pthread_cond_signal( cvptr[*currThread] ); // sends signal to next spy thread
		}
		
	}

return 1;
}
