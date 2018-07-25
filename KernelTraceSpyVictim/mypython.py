#!/usr/bin/env python
import numpy as np
import pylab as P
import csv
import sys


# pids_context_switch_statistics.txt

outfilename1 = sys.argv[1] 
ofile=(open(outfilename1, 'w'))
#ofile.write("p_pid\tp_vrt\tp_sum_exe_rt\tn_pid\tn_vrt\tn_sum_exe_rt\t|\tDiff_vrtime sum_rt_next\n")
ofile.write("p_pid	p_vrt	      p_sum_exe_rt   n_pid    n_vrt  	n_sum_exe_rt|	prev_tslice next_tslice  CPU\n")


# All_context_switch_statistics.txt
ofile2=(open("All_context_switch_statistics.txt", 'w'))
#ofile.write("p_pid\tp_vrt\tp_sum_exe_rt\tn_pid\tn_vrt\tn_sum_exe_rt\t|\tDiff_vrtime sum_rt_next\n")
ofile2.write("p_pid	p_vrt	      p_sum_exe_rt   n_pid    n_vrt  	n_sum_exe_rt|	prev_tslice next_tslice CPU\n")



pids=[]
ipcfilename = sys.argv[2]
with open(ipcfilename) as ipcfile:
	lines_ = ipcfile.readlines()
	for line_ in lines_:
		pids.append(int(line_.split()[1]))
		
print pids

with open ("context_switch.txt") as ifile:
	lines = ifile.readlines()
	cflag1=0;
	cflag2=0;

	vrun_time = {}
	vrun_time2 = {}

	vrun_time3 = {}
	vrun_time2 = {}
	for line in lines:
		if len(line.split(',')) is not 6:continue
		ar = line.split(',')
		
		last_vrun = 0
		last_vrun2 = 0
		if (ar[0].isdigit()):			
			if int(ar[0]) in vrun_time : 
				last_vrun = vrun_time[int(ar[0])]
				cflag1=1

			if last_vrun is 0 and cflag1 is 0:	
				cpu_time_spent= 0
			else:
				cpu_time_spent= int(ar[2]) - last_vrun
			vrun_time[int(ar[0])]=int(ar[2])

			if int(ar[3]) in vrun_time2 : 
				last_vrun2 = vrun_time2[int(ar[3])]
				cflag2=1
				

			if last_vrun2 is 0 and cflag2 is 0:	
				cpu_time_spent2= 0
			else:
				cpu_time_spent2= int(ar[5]) - last_vrun2
			vrun_time2[int(ar[3])]=int(ar[5])
 
			res='%6s , %15s, %15s, %6s , %15s,%15s, | , %10s ,%10s \n' % (ar[0], ar[1],ar[2],ar[3],ar[4],ar[5].strip(), str(cpu_time_spent), str(cpu_time_spent2))
			ofile2.write(res)
			
			if int(ar[0]) in pids: #   or int(ar[3]) in pids:
				ofile.write(res)
	

			 
ofile.close()
ofile2.close()

			
			

