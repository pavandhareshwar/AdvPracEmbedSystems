#include <linux/init.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/sched/signal.h>
#include <linux/kthread.h>
#include <linux/kfifo.h>
#include <linux/sched.h>
#include <linux/mutex.h>
#include <linux/slab.h>

#include <asm/current.h>

#define KFIFO_SIZE 				128
#define PID_INFO_SIZE			64

static DEFINE_MUTEX(kfifo_mutex);

MODULE_LICENSE("Dual BSD/GPL");
/* This was added to fix an error on version magic */
MODULE_INFO(vermagic, "4.14.15 SMP mod_unload ");

static struct task_struct *thread1_task_struct, *thread2_task_struct;
static struct kfifo ktc_fifo;

static int thread1_fun(void *data)
{
	/* 	Allow the thread to receive kill signal.
		By default, kernel threads ignore the signals 
	*/
	allow_signal(SIGKILL);

	char *prev_pid_info = NULL;
	char *curr_pid_info = NULL;
	char *next_pid_info = NULL;

	if ((prev_pid_info = kmalloc(PID_INFO_SIZE, GFP_KERNEL)) == NULL)
		return -ENOMEM;
	
	if ((curr_pid_info = kmalloc(PID_INFO_SIZE, GFP_KERNEL)) == NULL)
		return -ENOMEM;
	
	if ((next_pid_info = kmalloc(PID_INFO_SIZE, GFP_KERNEL)) == NULL)
		return -ENOMEM;


	/* Some test message for testing the kfifo */
	//struct { unsigned char buffer[10]; } msg = { "test msg" };
	
	while(!kthread_should_stop())
	{
		if (false == kfifo_is_full(&ktc_fifo))
		{
			struct task_struct *task = current;
		
			/* 	Computing the PID and vruntime of previous, current 
				and next processes in the rb tree and sending this 
				info via kfifo to another thread 
			*/
			
			sprintf(prev_pid_info, "Previous PID: %d, vruntime: %llu\n",
				list_prev_entry(task, tasks)->pid, list_prev_entry(task, tasks)->se.vruntime);
			
			sprintf(curr_pid_info, "Current PID: %d, vruntime: %llu\n",
				list_prev_entry(task, tasks)->pid, list_prev_entry(task, tasks)->se.vruntime);
			
			sprintf(next_pid_info, "Next PID: %d, vruntime: %llu\n",
				list_prev_entry(task, tasks)->pid, list_prev_entry(task, tasks)->se.vruntime);
	
			mutex_lock(&kfifo_mutex);		
	
			//kfifo_in(&ktc_fifo, &msg, sizeof(msg));
			kfifo_in(&ktc_fifo, prev_pid_info, strlen(prev_pid_info));
			kfifo_in(&ktc_fifo, curr_pid_info, strlen(curr_pid_info));
			kfifo_in(&ktc_fifo, next_pid_info, strlen(next_pid_info));
			
			mutex_unlock(&kfifo_mutex);		
		}
		ssleep(5);
		
		/* Check if the signal is pending */
		if (signal_pending(thread1_task_struct))
			break;
	}	

	printk(KERN_INFO "Stopping thread\n");
	if (prev_pid_info)
		kfree(prev_pid_info);
	
	if (curr_pid_info)
		kfree(curr_pid_info);
	
	if (next_pid_info)
		kfree(next_pid_info);

	do_exit(0);	

	return 0;
}

static int thread2_fun(void *data)
{
	/* 	Allow the thread to receive kill signal.
		By default, kernel threads ignore the signals 
	*/
	char *read_buffer = NULL;
	int i;
	allow_signal(SIGKILL);

	if ((read_buffer = kmalloc(PID_INFO_SIZE, GFP_KERNEL)) == NULL)
		return -ENOMEM;
	
	while(!kthread_should_stop())
	{
		if (false == kfifo_is_empty(&ktc_fifo))
		{
			mutex_lock(&kfifo_mutex);
			i = kfifo_out(&ktc_fifo, read_buffer, PID_INFO_SIZE);
			mutex_unlock(&kfifo_mutex);
			printk(KERN_INFO "Data read off kfifo: %.*s", i, read_buffer);	
		}
		ssleep(1);

		/* Check if the signal is pending */
		if (signal_pending(thread2_task_struct))
			break;
	}	

	printk(KERN_INFO "Stopping thread\n");

	if (read_buffer)
		kfree(read_buffer);

	do_exit(0);	

	return 0;
}

static int alloc_kfifo(void)
{
	int ret_val;

	ret_val = kfifo_alloc(&ktc_fifo, KFIFO_SIZE, GFP_KERNEL);
	
	return 0;
}

static int create_kthreads(void)
{
	/* Kernel thread 1 creation */	
	thread1_task_struct = kthread_create(thread1_fun, NULL, "myKThread1");

	if (thread1_task_struct)
	{
		wake_up_process(thread1_task_struct);
		printk(KERN_INFO "Kernel thread 1 started\n");
	}
	else
	{
		printk(KERN_INFO "Kernel thread 1 creation failed\n");
		return -1;
	}
	
	/* Kernel thread 1 creation */	
	thread2_task_struct = kthread_create(thread2_fun, NULL, "myKThread2");

	if (thread2_task_struct)
	{
		wake_up_process(thread2_task_struct);
		printk(KERN_INFO "Kernel thread 2 started\n");
	}
	else
	{
		printk(KERN_INFO "Kernel thread 2 creation failed\n");
		return -1;
	}

	return 0;
}

/* Module initialization function */
static int __init thread_init_fun(void)
{
	int thread_creat_ret_val = -1;

	printk(KERN_INFO "Creating kernel thread\n");

	thread_creat_ret_val = create_kthreads();

	if (thread_creat_ret_val != 0)
	{
		printk(KERN_INFO "Kernel thread creation failed\n");
		return thread_creat_ret_val;
	}	
	else
	{
		alloc_kfifo();	
	}

	return 0;
}

static void __exit thread_exit_fun(void)
{
	if (thread1_task_struct)
	{
		kthread_stop(thread1_task_struct);
		printk(KERN_INFO "Kernel thread 1 stopped\n");
	}
	
	if (thread2_task_struct)
	{
		kthread_stop(thread2_task_struct);
		printk(KERN_INFO "Kernel thread 2 stopped\n");
	}
		
	kfifo_free(&ktc_fifo);
}

module_init(thread_init_fun);
module_exit(thread_exit_fun);
