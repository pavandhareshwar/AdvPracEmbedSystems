/*****************************************************************
* Author: Pavan Dhareshwar
* Date: 3/6/2018
* File: process_tree_lineage.c
* Description: This source file creates and defines the functionality
*              of what the kernel module was supposed to do, but in 
*              user space. 
******************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <sys/procfs.h>
#include <sys/signal.h>
#include <sys/syscall.h>
#include <sys/param.h>
#include <linux/sched.h>

#include <dirent.h>

void find_parent_process(int proc_id)
{
	FILE *fp;
	prstatus_t status;
	
	if (proc_id == 1)
		return;

	char proc_dir_name[] = "/proc/";
	char proc_pid_path_name[100];
	char proc_pid_status_str[100];
	char proc_pid_task_str[100];
	char pid_string[20];
	int parent_pid;
	int num_threads = 0;

	memset(proc_pid_path_name, '\0', sizeof(proc_pid_path_name));
	memset(pid_string, '\0', sizeof(pid_string));

	sprintf(pid_string, "%d", proc_id);
	strcpy(proc_pid_path_name, proc_dir_name);
	strcat(proc_pid_path_name, pid_string);
	
	strcpy(proc_pid_status_str, proc_pid_path_name);
	strcat(proc_pid_status_str, "/status");
	
	strcpy(proc_pid_task_str, proc_pid_path_name);
	strcat(proc_pid_task_str, "/task");	

	printf("Opening file %s\n", proc_pid_status_str);
	if((fp = fopen(proc_pid_status_str, "r")) == NULL) 
		perror("fopen");
    
	char *buffer;
	size_t num_bytes = 120;
	ssize_t bytes_read;
	char colon_delimiter[] = ":";

	buffer = (char *)malloc(num_bytes * sizeof(char));

	printf("\n============================================================\n");	
	printf("Metrics: \n");
	
	while ((bytes_read = getline(&buffer, &num_bytes, fp)) != -1)
	{
		char *token;

		token = strtok(buffer, colon_delimiter);
		
		if (!(strcmp(token, "State")))
		{
			token = strtok(NULL, colon_delimiter);
			printf("State: %s", token);
		}
		
		if (!(strcmp(token, "Threads")))
		{
			token = strtok(NULL, colon_delimiter);
			num_threads = atoi(token);
			printf("Num of threads: %d\n", num_threads);
		}
	
		if (!(strcmp(token, "PPid")))
		{
			token = strtok(NULL, colon_delimiter);
			parent_pid = atoi(token);
			printf("PPID: %d\n", atoi(token));
			
		}
		
	}

	char proc_pid_task_pid_str[100];
	memset(proc_pid_task_pid_str, '\0', sizeof(proc_pid_task_pid_str));
	DIR *dir;
	struct dirent *dir_ent;

	printf("Threads: ");				
	dir = opendir(proc_pid_task_str);
	while ((dir_ent = readdir(dir)) != NULL)
	{
		if (strcmp(dir_ent->d_name, ".") && strcmp(dir_ent->d_name, ".."))
		{
			char thread_name[100];
			memset(thread_name, '\0', sizeof(thread_name));

			sprintf(proc_pid_task_pid_str, "%s%s%s%s%s", proc_pid_task_str, "/", dir_ent->d_name, "/", "comm");
			FILE *fp_thread_name = fopen(proc_pid_task_pid_str, "r");
			
			fread(thread_name, sizeof(thread_name), sizeof(char), fp_thread_name);
			fclose(fp_thread_name);

			if (thread_name[strlen(thread_name)-1] == '\n')
				thread_name[strlen(thread_name)-1] = '\0';

			printf("%s", thread_name);
	
			long int nice_val;
			char proc_pid_task_pid_stat_str[100];
			memset(proc_pid_task_pid_stat_str, '\0', sizeof(proc_pid_task_pid_stat_str));
			
			sprintf(proc_pid_task_pid_stat_str, "%s%s%s%s%s", proc_pid_task_str, "/", dir_ent->d_name, "/", "stat");
			FILE *fp_stat = fopen(proc_pid_task_pid_stat_str, "r");

			fscanf(fp_stat, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*lu %*lu %*lu %*lu %*lu %*lu %*ld %*ld %*ld %ld", &nice_val);

			printf("(nice_val : %ld) ", nice_val);

			fclose(fp_stat);
		}
	}

	printf("\n============================================================\n");	

	printf("parent_pid: %d\n", parent_pid);	
	find_parent_process(parent_pid);

	if (buffer)
		free(buffer);

	if (fp)
		fclose(fp);

}

int main(void)
{
	printf("Process ID : %d\n", getpid());
	printf("Parent process ID: %d\n", getppid());
	
	find_parent_process(getpid());
	
	printf("\n");

	return 0;
}
