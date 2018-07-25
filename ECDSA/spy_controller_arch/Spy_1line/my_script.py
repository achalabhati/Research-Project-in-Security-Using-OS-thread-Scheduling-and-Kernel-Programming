# this will detect initial FIXED DAAA sequnce and remaining bits will be returned as experimental key which will be compared


import sys
lines = []
filename=sys.argv[1]

with open(filename,'r') as f:
	lines = f.readlines()
size = len(lines)

ans = ""
op = ''
counter=0
last=0
d=0
ad=0
for i in range(size):
	sp = []
	sp = map(int,lines[i].split(','))
	#a = 0
	#d = 0
	#ad = 0
	if(sp[2]!=last):
		last=sp[2]
		ans = ans + op
		op=''
		d=0
		ad=0
		
	for j in range(3,5):
		
		#print "sp[",j,"]=",sp[j]
#		a = a + sp[j]		
		if(j < 4):
			d = d + sp[j]
		if(j > 3):
			ad = ad + sp[j]
#	if(a==0):
#		counter=counter+1		
#	if(a == 0 and counter>=5):
#		ans = ans + op
#		op = ''
#		counter=0
	if(d > 0 and ad > 0):
		op = "DA"
	elif(d > 0):
		op = 'D'
#		counter=0
	elif(ad > 0):
		#print "here"
		#print counter
		#print lines[i]
		op = 'A'
#		counter=0
	#print "ans:",ans	

size_ans=len(ans)
#print size_ans
#print ans
detected_seq=ans

# if ended with MM , discard last MM
#if ans[size_ans-1] =='M' and ans[size_ans-2] =='M':
	#print "Ended with MM"
#	detected_seq=ans[:-2]
# if ended with SM, discard last M	
#if ans[size_ans-1] =='M' and ans[size_ans-2] =='S':
	#print "Ended with SM"
#	detected_seq=ans[:-1] 
	

#if ended with SS , 
#if ans[size_ans-1] =='S' and ans[size_ans-2] =='S':
#	detected_seq=ans

print detected_seq

#find first occurance of MS in MSMMMMMMMS detected_seq
#s=detected_seq.index('MS',0)

#find 2nd occurance of MS in MSMMMMMMMS detected_seq
#s2=detected_seq.index('MS',s+1)

#print final detected sequence after discarding initial MSMMMMMMMM 
#final_detected_seq=detected_seq[s2:]

# If experimental detected sequence contains MM or MMM ( more than one M continuously)
# then replace MM or MMM occurances by M  as two/more consecutive M can never performed

#final_detected_seq = final_detected_seq.replace('MMMM', 'M')
#final_detected_seq = final_detected_seq.replace('MMM', 'M')
#final_detected_seq = final_detected_seq.replace('MM', 'M')
#print final_detected_seq

'''
# discard 1st 10 FIXED opearion sequence MSMMMMMMMM 
sub1_str1=detected_string[:10]
sub1_str2=detected_string[10:12]

if(sub1_str2=='MS'):
	final_detected_string=detected_string[10:]

if(sub1_str2=='MM'):
	sub1_str2=detected_string[11:13]
	if(sub1_str2=='MS'):
		final_detected_string=detected_string[11:]

print final_detected_string
'''
