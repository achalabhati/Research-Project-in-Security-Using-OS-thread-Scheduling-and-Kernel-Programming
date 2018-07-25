#!/bin/bash

#rm ori_seqeunce.txt
#rm ori_wNAF.txt
#rm ephemeral_key.txt
rm dbl_times.txt
rm add_times.txt
rm gap_times.txt

dir2='result_5000_v2/'

 gcc -O0 -Wall -g -lssl -UOPENSSL_NO_EC ecdsa.c -lcrypto -lrt -fPIC -o ecdsa

for KEY in {1..500}
do
	
 	./ecdsa

done
 #awk 'NR % 2==0' ori_seqeunce.txt > my_seq.txt
 #awk 'NR % 2==0' ori_wNAF.txt > my_wNAF.txt
awk '{ sum += $1 } END { if (NR > 0) print sum / NR }' 'dbl_times.txt' >  $dir2/'avg_dbl_time.txt'
awk '{ sum += $1 } END { if (NR > 0) print sum / NR }' 'add_times.txt' >  $dir2/'avg_add.txt'



