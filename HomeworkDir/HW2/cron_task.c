/**********************************************************
* Author: Pavan Dhareshwar
* Date: 2/1/2018
* File: cron_task.c
* Description: Program run as part of the cron task
***********************************************************/

#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "test_sys_and_lib_calls.h"

int main(void)
{	
	printf("This is a program to track system and library calls with file I/O\n");

	char file_name[] = "apes_hw2_q1_test_file.txt";
	FILE *fp = fopen(file_name, "w");
	if (!fp)
	{
		//printf("%s %d: File open failed\n", __func__, __LINE__);
		PRINT("File open failed\n");
		//exit(1);
	}	

	fclose(fp);

	struct stat file_info;
	stat(file_name, &file_info);

	printf("File permissions: %lo\n", (unsigned long)file_info.st_mode);

	if (chmod(file_name, (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)) != 0)
	//if (chmod(file_name, (S_IRUSR)) != 0)
	{
		PRINT("%s %d: chmod failed\n", __func__, __LINE__);
	}
	
	stat(file_name, &file_info);

	printf("File permissions(after): %lo\n", (unsigned long)file_info.st_mode);

	char ch = 'A';
	fp = fopen(file_name, "w");
	if (!fp)
	{
		PRINT("File open failed in write mode\n");
	}
	else
	{
		fputc(ch, fp);
		fclose(fp);
	}

	char *input_string = (char *)malloc(100*sizeof(char));
	if (!input_string)
	{
		PRINT("Malloc failed\n");
	}
	else
	{
		printf("Enter a string:\n");
		scanf("%s", input_string);
		
		fp = fopen(file_name, "a");
		if (!fp)
		{
			PRINT("File open in append mode failed\n");
		}
		else
		{
			fwrite(input_string, sizeof(char), strlen(input_string), fp);
			fflush(fp);
			fclose(fp);
		
			fp = fopen(file_name, "r");
			if (fp)
			{
				int c = fgetc(fp);
				printf("Character read: %c\n", c);

				char str_read[20];
				if (fgets(str_read, 20, fp) != NULL) 
				{
					puts(str_read);
				}

				fclose(fp);
			}
		}
	
	}


	if (input_string)
	{
		free(input_string);
	}

	pid_t cur_proc_id = getpid();
	printf("Current process ID: %d\n", cur_proc_id);

	uid_t cur_user_id = getuid();
	printf("Current user ID: %d\n", cur_user_id);

	time_t cur_time;
	time(&cur_time);

	printf("Current date and time: %s\n", ctime(&cur_time));

	int array[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int sorted_array[10];
    int size = sizeof(array)/sizeof(array[0]);

    syscall(333, array, size, sorted_array);

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
