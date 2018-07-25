
o_file=open('result.txt',"w")
with open("Sorted_probe2.txt") as fname:
	lines = fname.readlines()
	size = len(lines)
	#print size
	ccount=0
	sqr_counter = 0
	m_counter=0	
	for num in range(0, size-1):
		if(int(lines[num].split(',')[7])==1):
			#print lines[num]
			#print 'M', 
			o_file.write("M")
			m_counter=num
			num =num + 1 
			while True :						
				if(int(lines[num].split(',')[3])==1):
					if(int(lines[num].split(',')[4])==1 and int(lines[num].split(',')[5])==1 ):					
						if(num-sqr_counter)>5:
						#print 'S', 
							o_file.write("S")
							sqr_counter=num			
						else:
							sqr_counter=num	
				if(int(lines[num].split(',')[7])==1):
					if(num-m_counter)>5:
						m_counter=num
						#print 'M', 
						o_file.write("M")
					else:
						m_counter=num
									
				num =num + 1 
				if(num==size):
					break;				
		if(num==size):
			break;				
					
o_file.close()
