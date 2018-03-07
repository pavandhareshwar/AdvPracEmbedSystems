#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/slab.h>

#include <asm/current.h>
#include <linux/string.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("Kernel module to evaluate process tree");
MODULE_AUTHOR("Pavan Dhareshwar");

/* This was added to fix an error on version magic */
MODULE_INFO(vermagic, "4.14.15 SMP mod_unload ");

#define THREAD_NAME_LEN		50

int proc_id = -1;
/* Module Parameter */
module_param(proc_id, int, S_IRUGO);

struct pid *pid_struct;
struct task_struct *task;

static int find_process_lineage(struct task_struct *task)
{

	printk(KERN_INFO "========================================================\n");	

	int pid = task->pid;

	int nice_val = task_nice(task);

	char *thread_name = NULL;
	if ((thread_name = kmalloc(THREAD_NAME_LEN, GFP_KERNEL)) == NULL)
		return -ENOMEM;

	strcpy(thread_name, task->comm);

	int process_state = task->state;

	int num_childs = 0;
	struct list_head *list;
	
	/* Iterate over the list */
	list_for_each(list, &task->children)
	{
		num_childs++;
		
		//struct task_struct *child_task;

		/* Get next child */
		//child_task = list_entry(list, struct task_struct, children);
		//if (child_task->comm != '\0')
			//printk(KERN_INFO "Child name: %s\n", child_task->comm);
		
	}
	
	printk(KERN_INFO "Process ID: %d, Nice Value: %d, Thread name: %s, Process state: %s, Num of children: %d\n", pid, nice_val, thread_name, (process_state == 0) ? "runnable" : 
					((process_state > 0) ? "stopped" : "unrunnable"), num_childs);

	if (thread_name)
		kfree(thread_name);
	
	printk(KERN_INFO "========================================================\n");	

	return 0;
}

static int proc_list_info_init(void)
{
	printk(KERN_INFO "Proc tree info module loaded\n");

	if (proc_id == -1)
	{
		/* 	No valid process id was passed. Setting working task to
			current and finding its lineage 
		*/
		task = current;
	}
	else
	{
		/* 	A valid process id was passed as module parameter. We will
			first find the task from the given pid and find its lineage
		*/
		pid_struct = find_get_pid(proc_id);
		task = pid_task(pid_struct, PIDTYPE_PID); 
		
	}
	
	find_process_lineage(task);

	do 
	{
		task = task->parent;
		find_process_lineage(task);	
	}while (task->pid != 1);

	
	return 0;
}

static void proc_list_info_exit(void)
{
	printk(KERN_INFO "Proc tree info module unloaded\n");
	
}

module_init(proc_list_info_init);
module_exit(proc_list_info_exit);
