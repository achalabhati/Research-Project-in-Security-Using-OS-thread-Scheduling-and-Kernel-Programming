#!/bin/bash
#disable prefetcher
#./prefetch.sh
gcc -O0 -Wall -g -lssl -UOPENSSL_NO_EC ecdsa.c -lcrypto -lrt -fPIC -o ecdsa
gcc  -O0 -Wall -g -lssl -UOPENSSL_NO_EC my_spy.c -lcrypto -fPIC -lrt -lpthread -o spy
rm probe2.txt
	rm probe1.txt
	rm sorted_probe2_3.txt
	rm dbl_times.txt
	rm add_times.txt
	rm gap_times.txt
	g++ check_accuracy.cpp -o accuracy



dir2='result_5000_v2/'
dir='result_5000_v2/Sorted_Probe2'

mkdir -p $dir
mkdir -p $dir2
for KEY in {1..5}
do

	echo 
	echo " KEY "$KEY
	
	
	# free shared memory 
	if [ $(( $KEY%50 )) -eq 0 ]
	then
	./remove.sh
	fi
	
	# read KEY from file	
	ori_KEY=`sed -n "${KEY}p" ephemeral_key.txt`
	ori_KEY_size=${#ori_KEY}
	echo $ori_KEY

	# read original AD sequence from file	

	ori_seq=`sed -n "${KEY}p" my_seq.txt`
	ori_seq_size=${#ori_seq}	
	#echo $ori_seq			
	
	
	killall DSA spy
	# run ECDSA first 
	./ecdsa $ori_KEY &
	sleep 1
	./spy 
	python rearrange_v3.py

	
	exp_seq=`python my_script.py Sorted_probe2_3.txt`
	size=${#exp_seq}		
	#calculate accuracy rate
	d=`./accuracy $exp_seq $ori_seq`
	
	# ignore first DAAA
	final_exp_seq=${exp_seq:4:size}
	#echo $final_ori_seq
	
	echo $d			
	
	echo $final_exp_seq >> $dir2/'only_seq.txt'
	
	

	echo "k="$ori_KEY >> $dir2/'Accuracy_Result.txt'
	
	echo "Deduced_Seq="$exp_seq >> 	$dir2/'Accuracy_Result.txt'
	echo "Accuracy="$d >> 	$dir2/'Accuracy_Result.txt'
	
	echo $d >> $dir2/'Accuracy_rate.txt'
	mv Sorted_probe2_3.txt $dir'/Sorted_probe2_3_'$KEY'.txt'
	rm probe*.txt
	
	
	

	#rm $dir2/'reversed.txt'
	
done
python inverse.py $dir2/'only_seq.txt'
python print_nonzero.py $dir2/'reversed.txt'
awk '{ sum += $1 } END { if (NR > 0) print sum / NR }' $dir2/'Accuracy_rate.txt' >  $dir2/'Average.txt'
#awk '{ sum += $1 } END { if (NR > 0) print sum / NR }' 'dbl_times.txt' >  $dir2/'avg_dbl_time.txt'
#awk '{ sum += $1 } END { if (NR > 0) print sum / NR }' 'add_times.txt' >  $dir2/'avg_add.txt'

