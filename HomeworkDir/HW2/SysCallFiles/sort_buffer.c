/**********************************************************
* Author: Pavan Dhareshwar
* Date: 2/1/2018
* File: sort_buffer.c
* Description: This program implements the functionality of
* 	       the system call used to sort data array in 
*	       kernel mode.
***********************************************************/

#include <linux/init.h>
#include <linux/syscalls.h>

#include "sort_buffer.h"

asmlinkage void sys_kernel_bufsort(char *user_buffer, int buff_len, char *sorted_buffer)
{
	printk(KERN_INFO "kernel_bufsort syscall entry\n");

    printk(KERN_INFO "Size of buffer to be sorted: %d\n", buff_len);
    int32_t *kern_buffer = kmalloc(256, GFP_KERNEL);
    if (kern_buffer == NULL)
	{
		printk(KERN_ALERT "Malloc failed for sort buffer\n");
        return -ENOMEM;
	}

    if (copy_from_user(kern_buf, user_buffer, buff_len))
	{
		printk(KERN_ALERT "copy_from_user failed\n");
        return -EFAULT;
	}

    printk(KERN_INFO "Sort started\n");

    /* Sort the elements in the kernel buffer in an order from largest to smallest */
    int i, j;
    for (i = 0; i < buff_len-1; i++)
    {
        for (j = 0; j < n-(i+1); j++)
        {
            if (A[j] < A[j+1])
            {
                int temp = A[j];
                A[j] = A[j+1];
                A[j+1] = A[j];
            }
        }
    }
    
    printk(KERN_INFO "Sort completed\n");
    
    if (copy_to_user(sorted_buffer, kern_buffer, buff_len))
	{
		printk(KERN_ALERT "copy_to_user failed\n");
        return -EFAULT;
	}
    
    if (kern_buffer)
        kfree(kern_buffer);

    printk(KERN_INFO "kernel_bufsort syscall exit\n");

}
