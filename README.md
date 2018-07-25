# Research-Project-in-Security-Using-OS-thread-Scheduling-and-Kernel-Programming : 
This attack is performed on the digital signature algorithms ECDSA and DSA used in application like bitcoins.
Private key used during the signing process is deduced using this attack.

# cache Line Probing:
Since private key depends upon the sequence of operation performed during the signing process, add and double (in case of ECDSA) and Multiply and square (in case of DSA). Signing process bring these functions(add or double) in cache during execution. So we need to proble the cache line to detect the sequence of these operation. FLUSH + RELOAD technique is used for cache line probing.

# Exploiting Kernel CFS Scheduler :
In order to detect the sequence of operation performed by the signing process a spy process should execute with it in interleaved manner. For this we need to exploit the CFS(Completely Fair Schedular).

# Thread Architecture on Dual Core:
In this architecture we have a spy controller running on one core and a spy ring(a group of threads running in round robin fashion ) on another core[15]. These spy threads will be interleaved with that of the victim which is performing the ECDSA operation and whose ephemeral key is to be found out.These threads are executed in a way that the victim is preempted after some limited operations. Then it should be determined if the victim has brought some data into the cache lines which in turn reveals some information about the ephemeral key. The spy threads are executed one after another with victim running between execution of any two threads. The victim is preempted in short duration due to the Completely Fair Scheduler(CFS). If the spy threads are not present, the victim can run on the processor for long time and perform the signing process but by introducing the spy threads we are exploiting the CFS.
The CFS is employed in many Linux versions which keeps track of virtual runtimes to make sure that the overall CPU times allocated to all processes and threads are nearly equal. So the total time given to victim(V) is equal to the times given to each spy thread.
Suppose there are n threads and each thread is running for x duration. Due to CFS, victim will also be executed for x duration by the time all the threads complete their execution.
overall CPU running time = time taken by n spy threads to run + time taken by victim to execute
As the victim is interrupted after some time by the threads, the victim runs for x/n time in each uninterrupted run. As a result, the greater the value of n, the victim will be executed for shorter duration. The shorter the duration, lesser operations are performed by the victim and better results are obtained.

# Thread Architechture on Single Core:
This architecture performs attack on ECDSA using only spy threads unlike previous ar- chitecture that used spy controller for synchronization among threads. In this architecture there is only spy ring( group of spy threads ) that runs with the victim on the same core. The basic idea of the attack is the same i.e. to preempt victim at short durations to know the activities performed by the victim. The victim will run for x/n time in single run where x is the total duration of spy and n is the number of threads used in the spy ring.
In this architecture timers are used for synchronization among threads as opposed to mutex variables in earlier architecture. When a thread completes its execution it will start a timer and control goes from user mode to kernel mode. When timer expires the kernel will send a signal that will call the signal handler[9]. The signal handler will then signal the next thread to continue its execution. So the victim will execute in the between these threads before the next thread starts executing.


