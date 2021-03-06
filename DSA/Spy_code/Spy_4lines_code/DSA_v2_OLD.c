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



char key_str[][100]={
"1678F6AF4762F12C613861C80CB2CA7F135131A00",
"1059251DDA36DC9D2FED031016D9912819D45FD00",
"1434D5CCB3582CAAC29C0ECFB04BEF9BC59E7D900",
"18A399D27DED0D0D21F593FDE276ECE55E7CE7300",
"11E132988D7BC1EED3C13CD55DD79D499AB8EF900",
"187B2DBC84BCB489962B5856F3857B18AE6D87500",
"173445B94E9486F16079B441C67BD01944F832A00",
"179D87346EB9A42FD6CA324596E46A6323FC9EC00",
"1235B11B0872E4CA090F1CE59FEB24FCCA9F2B800",
"12CE73DF257FFAAE2BF81A9A7C88961B71269BC00",
"1254EAC1F14425D42207A9BBE2C2D957D077D4D00",
"1544B89473F2D5F33BCBAF14052AFCF656A434F00",
"16CB9A834CAFB4CA0BDF66F1626E0A9906843EB00",
"18897FB925B027A4C6D1E6DF0402D3A1BB8505200",
"15248D00F9B5A0B579937CEABFE8212EE5C465300",
"15C709EBBE4ABEC77129D7F073A1FEF5167B4D400",
"10A1DCC6205176B49E1AD42A069DF4F69F43A3C00",
"168FE604B03784EF435308649AD7EAECB74986300",
"110450C735FD147AED9FDD45D987D7CAB480E3200",
"1736F474D6AEA4AD2619C93D8ACA0A1DC88C4A100",
"159B11AA9774E2B6BDC74E8775A882F4405341800",
"16CC6F2A601AA3CEF95F70F3A0EFF64558998D000",
"17AA03E168B83960B4C1156CAFFD98FA2F3623800",
"1513A580E12F86C463FC84B2F967008DFAD8ACC00",
"166B0ABAD052B9FE1A74AC138768C0BEA0BB8C500",
"14D6035BE0B2A0BBA80B54A561A0FB0652566E700",
"12450AADCAD6A30DF7C77168059313BBF45560200",
"13440A133346ED226429EE52B5E6F945388F80500",
"10A516324D1714C5E45E4270B431CB4D67F97FE00",
"167A8A99A6E588F74AD1F50CA9FB5F352A5C38500",
"116F12D93A5A8A539A152C95BBC92A1D87BDB6C00",
"126891B3AD408FCF22059C6EF716BD8B80B3AB000",
"12838A230BB4D9F977AA30357685877229E916C00",
"11AACD9C3A38259C79517AD9CED65E8FD4F4BC300",
"1914890BD236FBF5A7A5B6C7136685CD70CFE5200",
"133F4B45865AACDFED2E08E3584394AA094662E00",
"154F50C46295627C90CC2E368D6C62F8F5FBE3F00",
"17C0C5EBB7C8F11683586A51A11E5524A2C06DE00",
"191A965FFA8017BE46D532DF313D6CFC90CBE7E00",
"16232E09FB9A28BFF0F617FFCE26AFEAF4B525300",
"161B4CB967FBDB6ACDBEA013A792A939677424300",
"12FD5FFCF9B0AEDE70DE9483B067B350A3D14B600",
"125FDAB9005150D8B8F7C9DD5D773020EF9480400",
"176EA030E472D28A61E7A232A14FD8DB16DC30300",
"1709BDF13D28E363C2BDCE9AE6D8EE756236A2800",
"133ED252B4957650FC65B08DFCD2ACAD552C60100",
"15A3ABC1DA1C8512A8FE2C03B62D12C886CE49D00",
"18274476F9D122A93F03B0D4C33D5EE0620190500",
"1013622657AEEF5DDB9C80398C109D414E6CB0E00",
"17EAB719BAE9691DE7CA22C04A711E25BF95A9200",
"103A86180177667B5847F7282AD6D7F0D7C452D00",
"13D6F69BCEE65EF784DFA3B29DD7E6AEF5A595800",
"11EB36B37DC65AEE603656F03266CAA4064D79000",
"129BD95A3F2D071F72F33D84E65DCA7FE66EFA900",
"1107B6DF7FC73ECB63F62D32C94E738436F18B600",
"18961398AA15F609D55E50AD0250BFA6AF6E3D400",
"154AD36684F670AA0F4089FB4FBA3EDA9B3781200",
"11C1D5A47D26842DF062862B265F7021FBC5E1200",
"18056D2814C98E8186553EB479391CC3AF9787E00",
"18A53CEC6FB50F5B4CC4DE3C36BCAFEFE988F4D00",
"11962256236621CF7D4E7DC4EBFD2E183C2444100",
"1140AFE33CF9BEAB778BB34D873657626066D1100",
"16A681275AB60ED35B8B7FD353A2DA5D4262E6A00",
"136C1CEFCFC2BF983D2DC3ECE11E1C1F6D0065800",
"131F870E782C34F2C07AAE5229C7B3555E064C300",
"15079389D73CF8A245018EFA8A288D841545EF600",
"19141687818FC45977806B9C369EADAF4866BF100",
"1374490E334A68BC1D0D1DE1E1B0ED08BE6E3C300",
"15E1232FA19F5D6DFD4A5B1293B1E032089B5F300",
"166A653E9D194911E54A0751D3120A7F3642E9A00",
"14DB3B19EC6B09E78137B5A34D68145FB2A7B2200",
"13062A936B483E46F7141C4D26F2E06B36B83EC00",
"17E9ED9417D4B4465F9BBC2E89C08D41623031B00",
"1161C36A0C975D5459AE07DC45708020E9C9D3700",
"1603D00EB2DDD0AA22D19A72D72769C0EA00EA600",
"1145C4628FEDCA8D611F4FB21855A95AAF985EC00",
"159F2FF9F1892B53201E144162AB732408BAE8100",
"1328005B6AC552B73F01B2C96887942494F359800",
"11ED403DCFEF757B6EC10B493A3F5D2A28D5A0300",
"17A89C7104BE26383B2FA6BB4009CD74BD0AA2000",
"127951CD17908E37DF81DFB69BB95F2CDE913E200",
"116BCC50C40D79BE1B5A92B40BF863F444A14B900",
"18471D122FC84E8763F55A11F04B27C4791BB5400",
"1001F6C48051C4F7D35476EAA0C1CE74207004A00",
"18D21FBCCC61371DC00B54613579FE91A49EA8200",
"10C8B513DC52FF834D3257D219628D6BC6EBCDD00",
"1754C7F68B8DBD85672680209AE06066068B41200",
"1169261AD291CAA065345E4D31D5327D2F17CF800",
"14243BEF48DBD4D10859DAA8B82587D998A85FE00",
"1821519AE52671EA197736EFB9CDA89005E58E300",
"11A346BBCAAAC8A9E871B40232FD6852C9FA19200",
"1462DB16374213AF77789BD2F060C12025D489600",
"111D96764E17F39BF3BD2DAFD4B11BEC89DAE3700",
"1166D187F3BE6AF1F2F5FF70DDECA615A6E4A8B00",
"158F098C09103E7D7ED378A48F9FEFBCB3AE50900",
"1211FF1D3E06176B210305F129D2FB92AF5D82C00",
"1296B62343D85BE8AF08E3AB5F15F7442EFC03200",
"158BDCB62F5E3F701F81BFCB86D20330C26E7D000",
"1288B241EEA422C90D993B7174F8BEFF831F79900",
"1068F7AB5925F66FEFDECABE41D36B1A4AB5DC300"
};

int main() {
	
	// Unlink first to clean  
    shm_unlink("/message1");
    shm_unlink("/message2");
    shm_unlink("/message3");

	create_shared_memory();
	
	// SET CPU AFFINITY
	cpu_set_t my_set;
	CPU_ZERO(&my_set);
	CPU_SET(2, &my_set); 
	sched_setaffinity(0, sizeof(cpu_set_t), &my_set); 
	
	FILE *fp3;
	fp3=fopen("dummy.out","w");
	fprintf(fp3,"dummy %d\n",getpid());
	fclose(fp3);
	 
 	DSA *parameters = DSA_generate_parameters(1024, NULL, 0, NULL, NULL, NULL,NULL);
	DSA_generate_key(parameters);
	
	printf("\nparameters->priv_key=");
	BN_print_fp(stdout,parameters->priv_key);
	printf("\n");
	
	printf("\nparameters->pub_key=");
	BN_print_fp(stdout,parameters->pub_key);
	printf("\n");
	
	
	printf("\nparameters->p=");
	BN_print_fp(stdout,parameters->p);
	printf("\n");
	
	
	printf("\nparameters->q=");
	BN_print_fp(stdout,parameters->q);
	printf("\n");
	
	
	
	printf("\nparameters->g=");
	BN_print_fp(stdout,parameters->g);
	printf("\n");
	
	
	// In case customized parameters need to be used to generate same sequence later
/*	

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
	
	
	printf("Modified keys ");
	printf("\nparameters->priv_key=");
	BN_print_fp(stdout,parameters->priv_key);
	printf("\n");
	
	printf("\nparameters->pub_key=");
	BN_print_fp(stdout,parameters->pub_key);
	printf("\n");
	
	
	printf("\nparameters->p=");
	BN_print_fp(stdout,parameters->p);
	printf("\n");
	
	
	printf("\nparameters->q=");
	BN_print_fp(stdout,parameters->q);
	printf("\n");
	
	
	
	printf("\nparameters->g=");
	BN_print_fp(stdout,parameters->g);
	printf("\n");
*/	
	
	
	char data[] = "This is a test string.";
	unsigned char hash[SHA_DIGEST_LENGTH];
	SHA1(data, sizeof(data), hash);

	// Hash message for signature.
	BIGNUM hash_msg;
	BN_init(&hash_msg);
	BN_bin2bn(hash, SHA_DIGEST_LENGTH, &hash_msg);

	int siglen;
	unsigned char *sigret = malloc(DSA_size(parameters));
  
	printf(" Waiting for DSA Signature to start ,*start_flag=%d\n",*start_flag);	
   // while(!*start_flag)
    {
		//usleep(1); 
	};
	printf("DSA Signature Started, *access_count=%d,*start_flag=%d\n",*access_count,*start_flag);	
	
	
  int KEY_NO=0;
  
  while(KEY_NO<1)
  {
	/* Value of Customized Ephemeral Key is passing here via shared memory*/
	shared_stuff->str=key_str[KEY_NO];	  
	DSA_sign(0, hash, SHA_DIGEST_LENGTH, sigret, &siglen, parameters); //Signing here	
	KEY_NO++;	 
  }	

 //printf("\nDSA Signature Finished, *access_count=%d\n",*access_count);	
	return 0;
}
