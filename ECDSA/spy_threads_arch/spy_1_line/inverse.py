#this file will inverse the double and add sequence




import sys
lines = []
filename=sys.argv[1]

output_file1=open('result_5000_v2/reversed.txt','a+') 

with open(filename,'r') as f:
	lines = f.readlines()
size = len(lines)

for i in range(size):
	 output_file1.write(lines[i][::-1])
	

			

