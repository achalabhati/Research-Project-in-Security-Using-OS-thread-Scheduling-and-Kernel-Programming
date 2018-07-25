#!/bin/bash
#disable prefetcher
#./prefetch.sh
gcc -O0 -Wall -g -lssl -UOPENSSL_NO_EC ecdsa.c -lcrypto -lrt -fPIC -o ecdsa
gcc  -O0 -Wall -g -lssl -UOPENSSL_NO_EC 1_my_spy.c -lcrypto -fPIC -lrt -lpthread -o spy
	rm probe2.txt
	rm probe1.txt
	rm sorted_probe2_3.txt
	
	g++ check_accuracy.cpp -o accuracy

SAMPLE_SET=1-5000

dir2='result_gauri_5000_v2/'$SAMPLE_SET
dir='result_gauri_5000_v2/'$SAMPLE_SET/'Sorted_Probe2'

mkdir -p $dir
mkdir -p $dir2



TOTAL_SAMPLE=0

	
	
for KEY in {1..20}
do

	echo 
	echo " KEY "$KEY
	
	
	# free shared memory 
	if [ $(( $KEY%50 )) -eq 0 ]
	then
	./remove.sh
	fi
	
	# read KEY from file	
	ori_KEY=`sed -n "${KEY}p" key.txt`
	ori_KEY_size=${#ori_KEY}
	echo $ori_KEY

	# read original AD sequence from file	

	ori_seq=`sed -n "${KEY}p" my_seq.txt`
	ori_seq_size=${#ori_seq}	
	#echo $ori_seq			
	
	# ignore first DAAA
	#final_ori_seq=${ori_seq:4:ori_seq_size}
	#echo $final_ori_seq
	killall DSA spy
	# run ECDSA first 
	./ecdsa $ori_KEY &
	sleep 1
	./spy 0 5000
	python rearrange_v3.py
	
	exp_seq=`python my_script.py Sorted_probe2_3.txt`
	size=${#exp_seq}		
	#calculate accuracy rate
	d=`./accuracy $exp_seq $ori_seq`
	
	echo $d			

	echo "k="$ori_KEY >> $dir2/'Accuracy_Result.txt'
	echo "Deduced_Seq="$exp_seq >> 	$dir2/'Accuracy_Result.txt'
	echo "Accuracy="$d >> 	$dir2/'Accuracy_Result.txt'
	
	echo $d >> $dir2/'Accuracy_rate.txt'
	mv Sorted_probe2_3.txt $dir'/Sorted_probe2_3_'$KEY'.txt'
	rm probe*.txt

	
done
