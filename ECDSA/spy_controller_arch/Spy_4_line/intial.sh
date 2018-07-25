#!/bin/bash

rm ori_seqeunce.txt
rm key.txt
 gcc -g -lssl -UOPENSSL_NO_EC ecdsa.c -lcrypto -o ecdsa

for KEY in {1..1000}
do
	
 	./ecdsa

done
 awk 'NR % 2==0' ori_seqeunce.txt > my_seq.txt




