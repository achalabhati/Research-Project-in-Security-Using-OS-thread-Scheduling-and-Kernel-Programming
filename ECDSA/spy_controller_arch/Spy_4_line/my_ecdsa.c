//compiled with gcc -g -lssl -UOPENSSL_NO_EC ecdsa.c -lcrypto
#include <openssl/ec.h>      // for EC_GROUP_new_by_curve_name, EC_GROUP_free, EC_KEY_new, EC_KEY_set_group, EC_KEY_generate_key, EC_KEY_free
#include <openssl/ecdsa.h>   // for ECDSA_do_sign, ECDSA_do_verify
#include <openssl/obj_mac.h> // for NID_secp256k1

//FILE *fp3;

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
		//fprintf(fp3,"after EC_KEY_generate_key ");

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
                    ECDSA_SIG *signature = ECDSA_do_sign(hash, strlen(hash), eckey);
			//printf("after do sign");
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

  return function_status;
}

int main( int argc , char * argv[] )
{
    unsigned char hash[] = "c7fbca202a95a570285e3d700eb04ca2";
    int status = create_signature(hash);
    return(0) ;
}
