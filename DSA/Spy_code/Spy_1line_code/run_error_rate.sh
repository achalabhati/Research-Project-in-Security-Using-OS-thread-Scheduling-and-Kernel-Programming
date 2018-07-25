# this is used to calculate accuracy

#!/bin/bash
#disable prefetcher
#./prefetch.sh

rm spy DSA probe*.txt *.out

gcc -O0 -Wall DSA_gen.c -lcrypto  -o DSA -fPIC -lrt
g++ -O0 -Wall spy_v2.cpp -lcrypto  -o spy -fPIC -lpthread -lrt
g++ check_accuracy.cpp -o accuracy

SAMPLE_SET=1-5000

dir2='result_gauri_5000_v2/'$SAMPLE_SET
dir='result_gauri_5000_v2/'$SAMPLE_SET/'Sorted_Probe2'

mkdir -p $dir
mkdir -p $dir2



TOTAL_SAMPLE=0

for KEY in {1..1}
do

	echo 
	echo " KEY "$KEY
	
	
	# free shared memory 
	if [ $(( $KEY%50 )) -eq 0 ]
	then
	./remove.sh
	fi
	

	
	# read KEY from file	
	ori_KEY=`sed -n "${KEY}p" Key_only_1`
	ori_KEY_size=${#ori_KEY}
	
	# ignore first " and ",
	final_ori_KEY=${ori_KEY:1:ori_KEY_size-3}

	# read original SM sequence from file	
	ori_seq=`sed -n "${KEY}p" Key_1_Ori_seq`
	ori_seq_size=${#ori_seq}				
	
	# ignore first MSMMMMMMMM
	final_ori_seq=${ori_seq:10:ori_seq_size}
	
	# read KEY from file	
	ori_KEY=`sed -n "${KEY}p" Key_only_1`
	ori_KEY_size=${#ori_KEY}
	# ignore first " and ",
	final_ori_KEY=${ori_KEY:1:ori_KEY_size-3}
	
	
	
	 
	killall DSA spy
	# run DSA first 
	./DSA $final_ori_KEY &
	sleep 1

	./spy 0 100 

	python rearrange_v3.py
	
	#generate SM sequence from experimental data ,ignore first MSMMMMMMMM
	exp_seq=`python script_v3.py Sorted_probe2_3.txt`
	size=${#exp_seq}				
	

	#calculate accuracy rate
	d=`./accuracy $exp_seq $final_ori_seq`
	
	echo $d			

	echo "k="$final_ori_KEY >> $dir2/'Accuracy_Result.txt'
	echo "Deduced_Seq="$exp_seq >> 	$dir2/'Accuracy_Result.txt'
	echo "Accuracy="$d >> 	$dir2/'Accuracy_Result.txt'
	
	echo $d >> $dir2/'Accuracy_rate.txt'
	mv Sorted_probe2_3.txt $dir'/Sorted_probe2_3_'$KEY'.txt'
	
	rm probe*.txt
	
done

