/**********************************************************
* Author: Pavan Dhareshwar
* Date: 2/1/2018
* File: test_sys_and_lib_calls.c
* Description: Program that performs predefined set of file
*	       operations to help track system and library
*	       calls made when its run.
***********************************************************/

#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>

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


	return 0;
	
}
