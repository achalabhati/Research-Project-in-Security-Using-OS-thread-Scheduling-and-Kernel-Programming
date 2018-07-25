//compiled with gcc -g -lssl -UOPENSSL_NO_EC ecdsa.c -lcrypto
#define _GNU_SOURCE   
#include <sched.h>
#include <stdio.h>
#include <openssl/ec.h>      // for EC_GROUP_new_by_curve_name, EC_GROUP_free, EC_KEY_new, EC_KEY_set_group, EC_KEY_generate_key, EC_KEY_free
#include <openssl/ecdsa.h>   // for ECDSA_do_sign, ECDSA_do_verify
#include <openssl/obj_mac.h> // for NID_secp256k1

#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <pthread.h>
#include <sched.h>
#include "ec_lcl.h"
#include <syscall.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdbool.h>
#ifndef _POSIX_THREAD_PROCESS_SHARED
#error This system does not support process shared mutex
#endif

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


FILE *fp3;

static int create_signature(unsigned char* hash)
{
	
    int function_status = -1;
    EC_KEY *eckey=EC_KEY_new();
	//fprintf(fp3,"after EC_KEY_new()");
    if (NULL == eckey)
    {
        printf("Failed to create new EC Key\n");
        function_status = -1;
    }
    else
    {
        EC_GROUP *ecgroup= EC_GROUP_new_by_curve_name(NID_secp256k1);
	//fprintf(fp3,"after EC_GROUP_new_by_curve_name");
        if (NULL == ecgroup)
        {
            printf("Failed to create new EC Group\n"); 
            function_status = -1;
        }
        else
        {
            int set_group_status = EC_KEY_set_group(eckey,ecgroup);
            
       
            
		//fprintf(fp3,"after EC_KEY_set_group ");
            const int set_group_success = 1;
            if (set_group_success != set_group_status)
            {
                printf("Failed to set group for EC Key\n");
                function_status = -1;
            }
            else
            {
                const int gen_success = 1;
                int gen_status = EC_KEY_generate_key(eckey);
                
                
                
           
           
             
   
		char *str1_priv_key="98B112BAC4CCD10D9C854C350256BDF4222A60176EB2A98169AD30AF08594901";
		BIGNUM *x1,*y1,*my_key;
		my_key=BN_new();
		BN_hex2bn(&my_key,str1_priv_key);
		BN_copy(eckey->priv_key,my_key);
	    
       	char *str1_pub_x="F19FFCC977231A85924DB2E30D2020E90C489D6AB2C93FB6BD7B96BD1E3644BE";
		x1=BN_new();
		BN_hex2bn(&x1,str1_pub_x);
		
		
		char * str1_pub_y="AD2DE57B8F9AB9289ED81C85AC9999B199E94BF7526668D80306B604D136CDDA";
		y1=BN_new();
		BN_hex2bn(&y1,str1_pub_y);
		 
		if(EC_POINT_set_affine_coordinates_GFp(ecgroup, eckey->pub_key, x1, y1, NULL)){
			printf("\n\n\nsuccesfull\n\n\n");
		}
		
                if (gen_success != gen_status)
                {
                    printf("Failed to generate EC Key\n");
                    function_status = -1;
                }
                else
                {
			//fp3=fopen("ori_seqeunce.txt","a");
			//fprintf(fp3,"\n");
			//fclose(fp3);
			//printf("before do sign");
		
		//printf("*******v***** Waiting for ECDSA Signature to start ,*start_flag=%d\n",*start_flag);	
			//*start_flag=1;
			while(!*start_flag);
		// printf("***********vvvv******ECDSA Signature Started, *access_count=%d,*start_flag=%d\n",*access_count,*start_flag);
		//s = ((char *)(&ec_GFp_simple_dbl)) +19;

                    ECDSA_SIG *signature = ECDSA_do_sign(hash, strlen(hash), eckey);
			
			//printf("********v*******after do sign access_count %d\n",*access_count);
			//fp3=fopen("ori_seqeunce.txt","a");
			//fprintf(fp3,"\n");
			//fclose(fp3);
                    if (NULL == signature)
                    {
                        printf("Failed to generate EC Signature\n");
                        function_status = -1;
                    }
                    else
                    {

                  /*      int verify_status = ECDSA_do_verify(hash, strlen(hash), signature, eckey);
                        const int verify_success = 1;
                        if (verify_success != verify_status)
                        {
                            printf("Failed to verify EC Signature\n");
                            function_status = -1;
                        }
                        else
                        {
                            printf("Verifed EC Signature\n");
                            function_status = 1;
                        }
	*/
                    }
                }
            }
            EC_GROUP_free(ecgroup);
        }
        EC_KEY_free(eckey);
    }
//printf("\n*****v**** exit sign");
  return function_status;
}

int main( int argc , char * argv[] )
{
	create_shared_memory();
	char KEY[100];
	//printf("\n********************** here acgakjljk *********************");
	strcpy(KEY,argv[1]);
	/* Value of Customized Ephemeral Key is passing here via shared memory*/
	shared_stuff->str=KEY;
	cpu_set_t my_set;
	CPU_ZERO(&my_set);
	CPU_SET(1, &my_set); 
	sched_setaffinity(0, sizeof(cpu_set_t), &my_set); 



    unsigned char hash[] = "c7fbca202a95a570285e3d700eb04ca2";
    int status = create_signature(hash);
//printf("\n****v*** victim end");
    return(0) ;
}
