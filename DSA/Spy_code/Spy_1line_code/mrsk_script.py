l1 = []
l2 = []

def smto01(line):
	result = line
	#result=result1 #[:-1]
#	print result
	size = len(result)
	
	s=0
	i=0
	j=0
	count_S=0
	count_M=0
	seq = ""
	while i < size:
		#print i
		while(i < size and result[i] == 'S'):
			 s += 1
			 i += 1
			 count_S+=1
		if(i < size and result[i] == 'M'):
			count_M+=1
			if(s < 4):
				seq += 'X'*(s-1)
				seq +=  '1'
			else:
				seq += '0'*(s-4)
				seq += 'XXX1'
			i = i + 1
			s=0
	if(s>0):
		seq += '0'*(s)
	'''
	print '\nSequence We Detected as\n'
	print seq	
	print "Detected key length="+ str(len(seq))
	print "Modified with respect to length"
	'''
	if(len(seq)==160):
		return "1"+seq
	elif(len(seq)==159):
		return "1X"+seq
	elif(len(seq)==158):
		return "1XX"+seq
	else:
		return seq
###########################################################################

with open("DSA_parameter") as f:
	l1 = f.readlines()
size1 = len(l1)
k1 = [str(l1[i][2:]).strip('\n') for i in range(size1) if i%3 == 0]
r = [str(l1[i][2:]).strip('\n') for i in range(size1) if i%3 == 1]
s = [str(l1[i][2:]).strip('\n') for i in range(size1) if i%3 == 2]
#print k1


with open("sample_data_total_match.txt") as f:
	l2 = f.readlines()
size2 = len(l2)
k2 = [str(l2[i][2:]).strip('\n') for i in range(size2) if i%2 == 0]
seq = [str(l2[i]).strip('\n') for i in range(size2) if i%2 == 1]
m = "7CB2545C5FF28D6421AD5FC1237400AFDE771187"

i = 0;
k2size = len(k2)
while i < k2size:
	#print i
	if k2[i] in k1:
		index = k1.index(k2[i])
		#print "here"
		seq01 = smto01(seq[i])
		#print "here2"
		print m + ' ' + str(r[index]) + ' ' + str(s[index]) + ' ' + seq01 #+ ' '+k2[i] #+' ' +seq[i]
		#print seq[i] + ' ' + k2[i] 
	i = i + 1

