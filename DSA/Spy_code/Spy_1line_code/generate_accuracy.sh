#!/bin/bash
#disable prefetcher
#./prefetch.sh

SAMPLE_SET=1-5000

dir2='result_5000/accuracy/'$SAMPLE_SET
dir='result_5000/1001-5000/Sorted_Probe2'

mkdir -p $dir
mkdir -p $dir2

for KEY in {1001..5000}
do
	echo 
	echo " KEY "$KEY
	
	# read KEY from file	
	ori_KEY=`sed -n "${KEY}p" Key_only_1`
	ori_KEY_size=${#ori_KEY}
	
	# ignore first " and ",
	final_ori_KEY=${ori_KEY:1:ori_KEY_size-3}
	
	FILE_NAME=$dir/'Sorted_probe2_3_'$KEY'.txt'
	
	#generate SM sequence from experimental data ,ignore first MSMMMMMMMM
	exp_seq=`python script_v4.py "${FILE_NAME}" `
	size=${#exp_seq}				
	
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
	
	
	echo "KEY : "$final_ori_KEY >> $dir2'/result_summery.txt'
	echo "Ori : "$final_ori_seq >> $dir2'/result_summery.txt'
	echo "Exp : "$exp_seq >> $dir2'/result_summery.txt'
	
	#calculate accuracy rate
	d=`./accuracy $exp_seq $final_ori_seq`
	echo $d >> $dir2'/accuracy.txt'
	
	echo "KEY="$KEY ",size="$size ",accuracy="$d  >> $dir2'/result_summery.txt'
	str=$size,$d
	SIZE_ACCURACY=$SIZE_ACCURACY,$str,"\n"
	if [ $KEY -eq 1 ] 
	then
		a=$d
	
	else	
		a=$a,$d
	fi		
	echo $d 
done
	echo  >> $dir2'/result_summery.txt'
	echo "accuracy for all runs "  >> $dir2'/result_summery.txt'
	echo $a >> $dir2'/result_summery.txt'
