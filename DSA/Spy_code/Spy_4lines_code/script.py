lines = []
with open("Sorted_probe2.txt") as f:
	lines = f.readlines()
size = len(lines)

ans = ""
op = ''
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
	if(a == 0):
		ans = ans + op
		op = ''
		
	elif(s > 0 and m > 0):
		op = "SM"
	elif(s > 0):
		op = 'S'
	elif(m > 0):
		#print "here"
		op = 'M'
	#print "ans:",ans	
		
print ans
	
