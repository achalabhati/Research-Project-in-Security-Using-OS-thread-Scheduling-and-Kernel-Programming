# this is used to generate results with same paramters p,q,g ,priv,pub 

#!/bin/bash
#disable prefetcher
#./prefetch.sh

rm spy DSA probe*.txt *.out

gcc -O0 -Wall DSA_gen.c -lcrypto  -o DSA -fPIC -lrt
g++ -O0 -Wall spy_v2.cpp -lcrypto  -o spy -fPIC -lpthread -lrt
g++ check_accuracy.cpp -o accuracy

SAMPLE_SET=1-5000

dir2='result_gauri_5000/'$SAMPLE_SET
dir='result_gauri_5000/'$SAMPLE_SET/'Sorted_Probe2'

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

	#convert float into integer
	d_int=${d%%.*}
	
	
	#store only accuracy 100 % , Error free samples
	if [ $d_int -eq 100 ]
	then
		echo "k="$final_ori_KEY >> sample_data_total_match.txt
		echo $exp_seq >> 	sample_data_total_match.txt
		mv Sorted_probe2_3.txt $dir'/Sorted_probe2_3_'$KEY'.txt'
		TOTAL_SAMPLE=$[$TOTAL_SAMPLE+1]
		
	fi

	rm probe*.txt
	echo "100% match"$TOTAL_SAMPLE 	
	
done

python mrsk_script.py > DSA_FINAL_RESULT
