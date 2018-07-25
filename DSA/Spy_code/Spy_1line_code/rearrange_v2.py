import csv

def getKey(item):
	return int(item[0])
def getKey1(item):
	return int(item[1])
def getKey2(item):
	return int(item[2])
	
	
cr=csv.reader(open('probe2.txt','rb'))
output_file1=open('Sorted_probe2.txt','w+') 

all_elements=[]
for row in cr:
	all_elements.append(row)
#all_elements=sorted(all_elements,key=getKey2)
all_elements=sorted(all_elements,key=getKey)
#all_elements=sorted(all_elements,key=getKey1)

for i in range(0,len(all_elements)):
	res=all_elements[i][0]+","+all_elements[i][1]+","+all_elements[i][2]+","+all_elements[i][3]+","+all_elements[i][4]+","+all_elements[i][5]+","+all_elements[i][6] +","+all_elements[i][7] +","+all_elements[i][8]+","+all_elements[i][9]+","+all_elements[i][10]+","+all_elements[i][11] +"\n"
	output_file1.write(res)		 	 	 

