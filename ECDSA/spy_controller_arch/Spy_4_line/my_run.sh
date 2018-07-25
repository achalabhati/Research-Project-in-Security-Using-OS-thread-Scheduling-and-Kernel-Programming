#!/bin/bash
#disable prefetcher
#./prefetch.sh
gcc -g -lssl -UOPENSSL_NO_EC ecdsa.c -lcrypto -lrt -o ecdsa
gcc -g -lssl -UOPENSSL_NO_EC 1_my_spy.c -lcrypto -lrt -lpthread -o spy

for KEY in {1..1}
do

	
	# read KEY from file	
	ori_KEY=`sed -n "${KEY}p" key.txt`
	ori_KEY_size=${#ori_KEY}
	#echo $ori_KEY

	# read original AD sequence from file	

	ori_seq=`sed -n "${KEY}p" my_seq.txt`
	ori_seq_size=${#ori_seq}	
	#echo $ori_seq			
	
	# ignore first DAAA
	final_ori_seq=${ori_seq:4:ori_seq_size}
	#echo $final_ori_seq
	
	./ecdsa $ori_KEY &
	sleep 1
	#echo "due to this********"
	./spy 0 500
	python rearrange_v3.py
	
	#python script_v3.py Sorted_probe2_3.txt

	
done
