import sys
lines = []
filename=sys.argv[1]

with open(filename,'r') as f:
	lines = f.readlines()
size = len(lines)

ans = ""
op = ''
counter=0
for i in range(size):
	sp = []
	sp = map(int,lines[i].split(','))
	a = 0
	s = 0
	m = 0
	for j in range(3,11):
		#print "sp[",j,"]=",sp[j]
		a = a + sp[j]		
		if(j < 7):
			s = s + sp[j]
		if(j > 6):
			m = m + sp[j]
	if(a==0):
		counter=counter+1		
	if(a == 0 and counter>=2):
		ans = ans + op
		op = ''
		counter=0
	elif(s > 0 and m > 0):
		op = "SM"
	elif(s > 0):
		op = 'S'
		counter=0
	elif(m > 0):
		#print "here"
		#print counter
		#print lines[i]
		op = 'M'
		counter=0
	#print "ans:",ans	

size_ans=len(ans)
#print size_ans
#print ans
# if ended with MM , discard last MM
if ans[size_ans-1] =='M' and ans[size_ans-2] =='M':
	#print "Ended with MM"
	print ans[:-2]
# if ended with SM, discard last M	
if ans[size_ans-1] =='M' and ans[size_ans-2] =='S':
	#print "Ended with SM"
	print ans[:-1] 
	

#if ended with SS , 
if ans[size_ans-1] =='S' and ans[size_ans-2] =='S':
	print ans
