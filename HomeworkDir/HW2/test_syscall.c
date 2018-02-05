/**********************************************************
* Author: Pavan Dhareshwar
* Date: 2/1/2018
* File: test_syscall.c
* Description: Program to test the functionality of the
*	       system call implemented to sort a data array
*	       in kernel mode.
***********************************************************/

#include <stdio.h>
#include <sys/syscall.h>
#include <linux/kernel.h>
#include <stdint.h>
#include <unistd.h>

int main(void)
{
	//int array[10] = {4, 6, 2, 5, 19, 3, 15, 12, 7, 11};
	int array[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	int sorted_array[10];
	int size = sizeof(array)/sizeof(array[0]);

	syscall(333, array, size, sorted_array);
	//syscall(333, array, size, NULL);

	printf("Input Array: ");
	for (int i = 0; i < 10; i++)
		printf("%d ", array[i]);

	printf("\n");	

	printf("Sorted Array: ");
	for (int i = 0; i < 10; i++)
		printf("%d ", sorted_array[i]);

	printf("\n");

	return 0;	
}
