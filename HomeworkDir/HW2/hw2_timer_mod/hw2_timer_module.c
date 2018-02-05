/**********************************************************
* Author: Pavan Dhareshwar
* Date: 2/1/2018
* File: hw2_timer_module.c
* Description: This program creates a kernel module that 
*	       creates and initializes a kernel timer and 
*	       upon expiry of the timer, prints out the 
*              number of times the timer fired to kernel 
*	       log buffers.
***********************************************************/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/timer.h>
#include <linux/jiffies.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("Kernel module to demonstrate the usage of kernel timer");
MODULE_AUTHOR("Pavan Dhareshwar");

static char *name = "Pavan Dhareshwar";
static int timer_freq = 500;

module_param(name, charp, S_IRUGO);
module_param(timer_freq, int, S_IRUGO);

struct timer_list hw2_timer;
unsigned long timer_expires_in;

void timer_module_func(unsigned long data)
{
	static int count;	

	printk(KERN_INFO "[%s] Timer expired %lu times\n", name, count++);

	/* Modifying the expiration of the already created timer */
	
	timer_expires_in = msecs_to_jiffies(timer_freq);
	mod_timer(&hw2_timer, jiffies + timer_expires_in);
}

static int timer_module_init(void)
{

	init_timer(&hw2_timer);

	timer_expires_in = msecs_to_jiffies(timer_freq);

	hw2_timer.expires = jiffies + timer_expires_in;
	hw2_timer.function = timer_module_func;
	hw2_timer.data = 1;

	add_timer(&hw2_timer);

	printk(KERN_INFO "Timer module loaded\n");

	return 0;
}

static void timer_module_exit(void)
{
	del_timer_sync(&hw2_timer);

	printk(KERN_INFO "Timer module unloaded\n");
} 

module_init(timer_module_init);
module_exit(timer_module_exit);
