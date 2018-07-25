#!/bin/bash

rm ori_seqeunce.txt
rm ori_wNAF.txt
rm ephemeral_key.txt
 gcc -O0 -Wall -g -lssl -UOPENSSL_NO_EC ecdsa.c -lcrypto -lrt -fPIC -o ecdsa

for KEY in {1..1000}
do
	
 	./ecdsa

done
 awk 'NR % 2==0' ori_seqeunce.txt > my_seq.txt
 awk 'NR % 2==0' ori_wNAF.txt > my_wNAF.txt




