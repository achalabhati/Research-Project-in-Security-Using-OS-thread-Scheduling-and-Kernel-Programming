# do not print lines before  the first probe detected , this is due to neglect those probe data when DSA not started

import csv

def getKey(item):
	return int(item[0])
def getKey1(item):
	return int(item[1])
def getKey2(item):
	return int(item[2])
	
	
cr=csv.reader(open('probe2.txt','rb'))
output_file1=open('Sorted_probe2_3.txt','w+') 

all_elements=[]
for row in cr:
	all_elements.append(row)

all_elements=sorted(all_elements,key=getKey2)
all_elements=sorted(all_elements,key=getKey1)
all_elements=sorted(all_elements,key=getKey)




start_flag=0
for i in range(0,len(all_elements)):
	#res=all_elements[i][0]+","+all_elements[i][1]+","+all_elements[i][2]+","+all_elements[i][3]+","+all_elements[i][4]+","+all_elements[i][5]+","+all_elements[i][6] +","+all_elements[i][7] +","+all_elements[i][8]+","+all_elements[i][9]+","+all_elements[i][10]+","+all_elements[i][11] +"\n"
	res=all_elements[i][0]+","+all_elements[i][1]+","+all_elements[i][2]+","+all_elements[i][3]+","+all_elements[i][4]+"\n"
	
	if(int(all_elements[i][0]) >4): # and (int(all_elements[i][4])+int(all_elements[i][5])+int(all_elements[i][6]) +int(all_elements[i][7]) +int(all_elements[i][8])+int(all_elements[i][9])+int(all_elements[i][10])+int(all_elements[i][11]))>0):
		start_flag=1	
	
	#if(int(all_elements[i][0]) >40):
	#	start_flag=0
			
	if(start_flag==1):
		output_file1.write(res)		 	 	 

