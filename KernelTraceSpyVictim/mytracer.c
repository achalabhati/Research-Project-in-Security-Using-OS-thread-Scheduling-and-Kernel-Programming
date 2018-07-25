#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/moduleparam.h>
#include<linux/pid.h>
#include<linux/sched.h>

//added by Bholanath
extern void (*mytracer_funptr)(u64 time1,struct task_struct *prev,struct task_struct *next);
extern void inline mytracer_info(u64 time1,struct task_struct *prev,struct task_struct *next);

// struct data structure to store runtime statistics
struct my_tracer {
        u64                     time_now; //rq_clock_task(rq_of(cfs_rq));
        pid_t                   PREV_pid,NEXT_pid;
        char					prev_name[100],next_name[100];
        u64                     PREV_exec_start,NEXT_exec_start;
        u64                     PREV_sum_exec_runtime,NEXT_sum_exec_runtime;
        u64                     PREV_vruntime,NEXT_vruntime;

};


extern struct my_tracer v_temp[20000];
extern void (*v_counter_reset_funptr)(void);
extern void inline v_counter_reset_info(void);

static int __init start(void)
{
	printk(KERN_INFO ", Started storing vruntime \n");
	v_counter_reset_funptr=&v_counter_reset_info;
	mytracer_funptr = &mytracer_info;
	return 0;
}

static void  stop(void)
{
	int counter;
	mytracer_funptr = NULL;
	v_counter_reset_funptr= NULL;
	for(counter=0;counter<20000;counter++)
		if(v_temp[counter].time_now>0)
		//printk(KERN_INFO "\ndddddddddddd %s\n",v_temp[counter].prev_name);
		printk(KERN_INFO ",%d, %llu,%d,%s,%llu,%llu,%llu,%d,%s,%llu,%llu,%llu\n",counter,v_temp[counter].time_now,v_temp[counter].PREV_pid,v_temp[counter].prev_name,v_temp[counter].PREV_exec_start,v_temp[counter].PREV_vruntime,v_temp[counter].PREV_sum_exec_runtime,v_temp[counter].NEXT_pid,v_temp[counter].next_name,v_temp[counter].NEXT_exec_start,v_temp[counter].NEXT_vruntime, v_temp[counter].NEXT_sum_exec_runtime);
	 
	printk(KERN_INFO " ,Stopped storing vruntime\n");
}


module_init(start);
module_exit(stop);
MODULE_LICENSE("GPL");
