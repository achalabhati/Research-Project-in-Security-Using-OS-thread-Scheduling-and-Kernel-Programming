
	
import sys
lines = []
filename=sys.argv[1]	
	
flag=0	
output_file1=open('result_5000_v2/non_zero.txt','w+') 
	
with open(filename,'r') as f1:
	for word in f1:
		for ch in word:
			if(flag==1):
				flag=0
				continue
			else:
				if(ch=='A'):
					#print 'N'
					output_file1.write('N')
					flag=1
				elif(ch=='D'):
					#print ch
					flag=0
					#print '0',
					output_file1.write('0')
		output_file1.write('N')		
		output_file1.write('\n')
	
			
