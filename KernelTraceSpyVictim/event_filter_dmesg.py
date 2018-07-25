#!/usr/bin/env python
import numpy as np
import pylab as P
import csv
import sys



#context_switch.txt

ofile1=(open("context_switch.txt", 'w'))
ofile1.write("prev_pid,prev_name,prev_vruntime,prev_sum_exec_runtime,next_pid,next_name,next_vruntime,next_sum_exec_runtime\n")

#input file, trace
infilename1 = sys.argv[1] 

with open (infilename1) as ifile:
	lines = ifile.readlines()
	cflag=0;
	vrun_time = {}
	for line in lines:
		#print len(line.split(','))
		#context_switch
		if len(line.split(',')) is 13:
			ar = line.split(',')
			res_=ar[3]+","+ar[4]+","+ar[6]+","+ar[7]+","+ar[8]+","+ar[9]+","+ar[11]+","+ar[12]
			#print(res_)
			ofile1.write(res_)
			ofile1.write('\n')
ofile1.close()
