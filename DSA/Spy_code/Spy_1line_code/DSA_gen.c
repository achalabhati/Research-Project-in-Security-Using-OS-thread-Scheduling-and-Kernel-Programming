/*
 * This program provides customized key ( ephemeral key) to generate DSA signature
 * No need to compile openssl again and again after modification of key as done previously
 * 
 * Shared memory is created and value of customized key ( ephemeral key) is assigned to 
 * shared variable "str" and in openssl before DSA signature is generated, value of K is modified with this value of 
 * shared variable "str"
 * 
 * Benefits : No need to change the value of K inside openssl source code and do make install after modification of key value 
 * */

///////////////////////// Includes /////////////////////////
///////////////////////// Openssl

#define _GNU_SOURCE             /* See feature_test_macros(7) */
#include <pthread.h>
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
#include <sys/stat.h>
#include <errno.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdbool.h>
#ifndef _POSIX_THREAD_PROCESS_SHARED
#error This system does not support process shared mutex
#endif



#include <stdio.h>
#include <openssl/dsa.h>
#include <openssl/err.h>
#include <openssl/sha.h>
#include <openssl/bn.h>
#include <stdint.h>
#include <time.h>
#include <sys/shm.h>



struct shared_use_st 
{
	char *str;
};
struct shared_use_st *shared_stuff;

int *start_flag;
int *access_count;
char *str_key;
 
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
	
	*start_flag=0;
	
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
	*access_count=0;


	int shmid;
	void *shared_memory=(void *)0;
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
///////////////////////// Main Function /////////////////////////

int main(int argc, char *argv[])
{
	FILE *fp;
    fp=fopen("dummy.out","a");    
    fprintf(fp,"dummy %d\n",getpid());  
    fclose(fp); 
    
	
	create_shared_memory();

	char KEY[100];
	
	strcpy(KEY,argv[1]);
	
	/* Value of Customized Ephemeral Key is passing here via shared memory*/
	shared_stuff->str=KEY;
	
	//printf("str_key in DSA_v2 initialized=%s\n",shared_stuff->str);


	//printf("DUMMY address of BN_mul in spy =%p\n", &BN_mul); //BN_mod_mul_montgomery
	//printf("DUMMY address of BN_sqr in spy =%p\n", &BN_sqr);
		
	// SET CPU AFFINITY
	cpu_set_t my_set;
	CPU_ZERO(&my_set);
	CPU_SET(2, &my_set); 
	sched_setaffinity(0, sizeof(cpu_set_t), &my_set); 
	
	 
 	DSA *parameters = DSA_generate_parameters(1024, NULL, 0, NULL, NULL, NULL,NULL);
	DSA_generate_key(parameters);
	
	// Modify Parameters to generate same r and s with same parameters
	
	
   	char *str1_priv_key="5405080D1AA4B147FF7AD857A582AC024EE3F311";
	BIGNUM *my_key;
	my_key=BN_new();
	BN_hex2bn(&my_key,str1_priv_key);
	BN_copy(parameters->priv_key,my_key);

   	char *str1_pub_key="C5F0DFC7FC6824F9F64873E08141687B6CE04AF3BC2B92673889B403D2FC1AC4DB8D5C16CC52AA4D6609FCAEF35DC7797A5F4B085A79E0AC7AB08F71EBDE0B67483B5960F8B171DD27BDB8E5703D34F0819659218EC9D36A49D06D2A216F05D64B75A03E37BA03E15353C7FD840F37A415AE51F803D7D3552A694EA584480E1";
	 
	my_key=BN_new();
	BN_hex2bn(&my_key,str1_pub_key);
	BN_copy(parameters->pub_key,my_key);



	char *str2_p="8D416A3CA22C1AB2B3CD96904BEAA9F99886C837346A9C5855A05425735389409AE10F8FF84F948367EE0CE88E01439AF9C4B533660B1BE6E0AA97CAE43ED27D00E5F79749B03E8750E68271EB3B8DB717E580D1A0963E1437FF78652D667034249A6A3C90C1E6D61F321742D83B7C00850A474CA1583634F53804F2EE393F4D";
	my_key=BN_new();
	BN_hex2bn(&my_key,str2_p);
	BN_copy(parameters->p,my_key);

	char *str2_q="AE7210CC9A30793463A5332FC5BAC4AE8263EEE7";
	my_key=BN_new();
	BN_hex2bn(&my_key,str2_q);
	BN_copy(parameters->q,my_key);
	
	char *str2_g="8EE276B3E85A3DCF45313CDB8A61350D596C45C0065A058A6307A5A8B3C4DCD5C1F8873A43D61C0294662DC5C884B9916B20BB0734CBB1A6B3E879605BFCA33CCCEBE58889A7E39B9310C92CD7CBBE2CFC274B29248A96E8837F1CEF89D7D000F09D98F8138605D1A8ECA7C5CBB5DB27EDDF5A03259599A7EBFA0EDAB4A3D38";
	my_key=BN_new();
	BN_hex2bn(&my_key,str2_g);
	BN_copy(parameters->g,my_key);
	
	
	
	
	
	char data[] = "This is a test string.";
	unsigned char hash[SHA_DIGEST_LENGTH];
	SHA1(data, sizeof(data), hash);

	// Hash message for signature.
	BIGNUM hash_msg;
	BN_init(&hash_msg);
	BN_bin2bn(hash, SHA_DIGEST_LENGTH, &hash_msg);

	int siglen;
	unsigned char *sigret = malloc(DSA_size(parameters));
  
	//printf(" Waiting for DSA Signature to start ,*start_flag=%d\n",*start_flag);	
    while(!*start_flag);

   // printf("DSA Signature Started, *access_count=%d,*start_flag=%d\n",*access_count,*start_flag);	
	
	
  int counter=0;
  
  //while(1) //counter<10)
  {
	//printf("Signature %d\n",counter);	  
	DSA_sign(0, hash, SHA_DIGEST_LENGTH, sigret, &siglen, parameters); //Signing here
	//shared_stuff->access_count++; // Next signature  	
	counter++;
	 
  }	

// printf("DSA Signature Finished, *access_count=%d\n",*access_count);	
	return 0;
}
