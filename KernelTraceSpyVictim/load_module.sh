
	
# linux-4.6.1/kernel/sched/core.c

make
dmesg -c > d
rm probe*.txt
rm sorted_probe2_3.txt
insmod mytracer.ko
#gcc -pthread spy.c -lrt -o spy
#gcc sample.c -o sample
gcc -O0 -Wall -g -lssl -UOPENSSL_NO_EC ecdsa.c -lcrypto -lrt -fPIC -o ecdsa
gcc  -O0 -Wall -g -lssl -UOPENSSL_NO_EC my_spy.c -lcrypto -fPIC -lrt -lpthread -o spy
./spy &
./ecdsa F67C772F98A1BD1FC12A54A1322DB4BAF7B2F6DD43AACA210EF4812B2D2AE367 &



sleep 3
#rm $dir2/'reversed.txt'
	


rmmod mytracer.ko
pkill spy
pkill ecdsa

dmesg > dmesg.txt

python event_filter_dmesg.py dmesg.txt 
python statistics_context_switch_v2.py  pids_dummy_context_switch_statistics.txt dummy.out

 


