#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <time.h>
#include <signal.h>


#include "multiThreadProg.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Incorrect usage\n");
        printf("Usage: ./multiThreadprog <log_file_name>\n");
        exit(1);
    }

	/* 	Variable to hold the log file name specified by the user */
    char log_file_name[FNAME_BUF_LEN];
    memset(log_file_name, '\0', sizeof(log_file_name));
	strncpy(log_file_name, argv[1], strlen(argv[1]));

    printf("Log File Name: %s\n", log_file_name);

    /* 	Check if the file exists, if not create a file with R+W permissions */
    if (!fopen(log_file_name, "r"))
    {
        printf("Log file %s doesn't exist. Creating one.\n", log_file_name);
        FILE *fp = fopen(log_file_name, "w");
        if (fp)
        {
            printf("Log file %s successfully created\n", log_file_name);
            fclose(fp);
        }
        else
        {
            printf("%s:%s:%d File open failed\n", __FILE__, __func__, __LINE__);
            exit(1);
        }
    }

	/* 	Create and initialize the thread info structure that will be passed on 
		to the master and two child threads
	*/
    x_thread_info thread_info;
    memset(&thread_info, '\0', sizeof(x_thread_info));

    strcpy(thread_info.log_file_name, log_file_name);

	/* Initialize the binary semaphore */
    sem_init(&g_sem_lfile_write, 0, 1);

    int thread_cret_ret_val = create_threads(thread_info);
    if (thread_cret_ret_val)
    {
        printf("%s:%s:%d Thread creation failed\n", __FILE__, __func__, __LINE__);
    }

    pthread_join(t_master_id, NULL);

    return 0;
}

int create_threads(x_thread_info thread_info)
{
    /* This function will create the master and two child threads. Each thread will be 
    ** passed a thread_info structure that contains the log file name that each thread
    ** will use to log information. 
    */
    
    int mt_creat_ret_val = pthread_create(&t_master_id, NULL, &mt_func, (void *)&thread_info);
    if (mt_creat_ret_val)
    {
        printf("Master thread creation failed\n");
        return -1;
    }
   
	//sleep(1);
 
    int ct1_creat_ret_val = pthread_create(&t_child_1_id, NULL, &ct1_func, (void *)&thread_info);
    if (ct1_creat_ret_val)
    {
        printf("Child thread 1 creation failed\n");
        return -1;
    }
    
	//sleep(1);

    int ct2_creat_ret_val = pthread_create(&t_child_2_id, NULL, &ct2_func, (void *)&thread_info);
    if (ct2_creat_ret_val)
    {
        printf("Child thread 2 creation failed\n");
        return -1;
    }

	sleep(1);

    return 0;
}

void *mt_func(void *arg)
{
    x_thread_info thread_info = *(x_thread_info *)arg;

	char buffer[MSG_BUF_LEN];
	memset(buffer, '\0', sizeof(buffer));

	fp_master = fopen(thread_info.log_file_name, "a");
	if (!fp_master)
	{
		printf("Log file open failed in master\n");
	}

	/* POSIX and linux thread id information */
	pthread_t posix_tid = pthread_self();
	pid_t linux_tid = syscall( __NR_gettid );

	/* Time when the thread started */
	time_t start_time = time(NULL); 
	char *time_str = ctime(&start_time);

	sprintf(buffer, "[Master] Thread start at %s (POSIX tid: %u, Linux tid: %d)\n", time_str, (unsigned int)posix_tid, linux_tid);
	write_to_log_file(buffer, strlen(buffer), fp_master);
	
	signal(SIGUSR1, sig_handler_ct1);
	//signal(SIGUSR2, sig_handler_ct1);
	//signal(SIGUSR1, sig_handler_ct2);
	signal(SIGUSR2, sig_handler_ct2);

	//pthread_kill(t_child_1_id, SIGUSR1);
	//pthread_kill(t_child_2_id, SIGUSR2);

    pthread_join(t_child_1_id, NULL);
    pthread_join(t_child_2_id, NULL);
	
	sprintf(buffer, "[Master] Thread exit\n");
	write_to_log_file(buffer, strlen(buffer), fp_master);
    
	fclose(fp_master);
	
	pthread_exit(NULL);

}

void sig_handler_ct1(int sig)
{
	char buffer[MSG_BUF_LEN];
	memset(buffer, '\0', sizeof(buffer));
	
	if (sig == SIGUSR1 || sig == SIGUSR2)
	{
		printf("Caught signal in child thread 1: %d\n", sig);

		/* 	Time when the thread started */
		time_t start_time = time(NULL); 
		char *time_str = ctime(&start_time);

		sprintf(buffer, "[Child 1] Thread exit at %s\n", time_str);
		write_to_log_file(buffer, strlen(buffer), fp_ct1);

		if (fp_ct1)
			fclose(fp_ct1);
	
		//pthread_exit(NULL);
		pthread_cancel(t_child_1_id);
	}
}

void sig_handler_ct2(int sig)
{
	char buffer[MSG_BUF_LEN];
	memset(buffer, '\0', sizeof(buffer));
	
	if (sig == SIGUSR1 || sig == SIGUSR2)
	{
		printf("Caught signal in child thread 2: %d\n", sig);
		
		/* 	Time when the thread started */
		time_t start_time = time(NULL); 
		char *time_str = ctime(&start_time);

		sprintf(buffer, "[Child 2] Thread exit at %s\n", time_str);
		write_to_log_file(buffer, strlen(buffer), fp_ct2);

		if (fp_ct2)	
			fclose(fp_ct2);
	
		//pthread_exit(NULL);
		pthread_cancel(t_child_2_id);

		return;
	}
}

char to_lower(char ch)
{
	if (ch >= 'A' && ch <= 'Z')
		return (ch - 'A' + 'a');
	return ch; 
}

void add_node_to_list(struct node_t **head, int c)
{
	struct node_t *current = (*head), *prev = NULL;

	if (head != NULL && current == NULL)
	{
		/* Linked list is empty */
		struct node_t *new_node = (struct node_t *)malloc(sizeof(struct node_t));
	
		new_node->ch = c;
		new_node->count = 1;
		new_node->next = NULL;

		(*head) = new_node;	
		return;	
	}

	while(current != NULL)
	{
		if ((char)current->ch == (char)c || (char)current->ch == to_lower((char)c))
		{	
			current->count++;
			break;
		}

		prev = current;
		current = current->next;
	}

	if (current == NULL)
	{
		/* 	The node corresponding to the character doesn't exist, we will be 
			creating one and setting the count as 1.	
		*/
		struct node_t *new_node = (struct node_t *)malloc(sizeof(struct node_t));
	
		new_node->ch = c;
		new_node->count = 1;
		new_node->next = NULL;

		prev->next = new_node;
	}

	return;
}

void printList(struct node_t *head)
{
	struct node_t *current = head;

	while (current != NULL)
	{
		printf("Character is %c with count %d\n", current->ch, current->count);
		current = current->next;
	}
}

void create_list(struct node_t **head)
{
	FILE *fp_txt_file = fopen(INP_TXT_FILE, "r");
	if (!fp_txt_file)
	{
		printf("File %s open failed\n", INP_TXT_FILE);
	}

	int c;

	do
	{
		c = fgetc(fp_txt_file);
		add_node_to_list(head, c);
		if (feof(fp_txt_file))
			break;
	}while(1);
}

void find_nodes_with_mult_occurrence(struct node_t *head)
{
	struct node_t *current = head;

	printf("Character with %d count: ", REP_COUNT);
	while(current != NULL)
	{
		if (current->count == REP_COUNT)
		{
			printf("%c ", (char)current->ch);
		}
		current = current->next;
	}
	printf("\n");
}

void *ct1_func(void *arg)
{
    x_thread_info thread_info = *(x_thread_info *)arg;

	char buffer[MSG_BUF_LEN];
    memset(buffer, '\0', sizeof(buffer));
	
	fp_ct1 = fopen(thread_info.log_file_name, "a");
	if (!fp_ct1)
	{
		printf("Log file open failed in child 1\n");
	}
	
	/* POSIX and linux thread id information */
	pthread_t posix_tid = pthread_self();
	pid_t linux_tid = syscall( __NR_gettid );

	/* 	Time when the thread started */
	time_t start_time = time(NULL); 
	char *time_str = ctime(&start_time);

	sprintf(buffer, "[Child 1] Thread start at %s (POSIX tid: %u, Linux tid: %d)\n", time_str, (unsigned int)posix_tid, linux_tid);
	write_to_log_file(buffer, strlen(buffer), fp_ct1);

	/* 	Go through the text file and create a linked list with a node for each 
	    character and store the character count in the count variable. Once the
		entire file is read and the linked list is updated, we traverse the list
		and find the character that has a count of 3(case is ignored).   
	*/
	while(1)
	{
		struct node_t *head = NULL;
		struct node_t **last_ref_ptr = &head;
		create_list(last_ref_ptr);

		//printList(*last_ref_ptr);

		find_nodes_with_mult_occurrence(*last_ref_ptr);
	
		sleep(2);
	}

}

void *ct2_func(void *arg)
{
    x_thread_info thread_info = *(x_thread_info *)arg;

	char buffer[MSG_BUF_LEN];
	memset(buffer, '\0', sizeof(buffer));
		
	fp_ct2 = fopen(thread_info.log_file_name, "a");
	if (!fp_ct2)
	{
		printf("Log file open failed in child 2\n");
	}

	/* 	POSIX and linux thread id information */
	pthread_t posix_tid = pthread_self();
	pid_t linux_tid = syscall( __NR_gettid );

	/* 	Time when the thread started */
	time_t start_time = time(NULL); 
	char *time_str = ctime(&start_time);

	sprintf(buffer, "[Child 2] Thread start at %s (POSIX tid: %u, Linux tid: %d)\n", time_str, (unsigned int)posix_tid, linux_tid);
	write_to_log_file(buffer, strlen(buffer), fp_ct2);

	/*	Varaible to hold the output of /proc/stat file read operation
		
		Reference: www.linuxhowtos.org/System/procstat.htm
	
		cpu user nice system idle iowait irq softirq
		
		The numbers captured identify the amount of time CPU has spent perfoming 
		different kind of work.
	
		cpu 	--> aggregrates the number in all the other "cpuN" lines
		
		user 	--> normal processes executing in user mode
		nice 	--> niced processes executing in user mode
		system	--> processes executing in kernel mode
		idle 	--> twiddling thumbs
		iowait 	--> waiting for I/O to complete
		irq		--> servicing interrupts
		softirq	--> servicing soft interrupts	
	 
	*/

	while(1)
	{
		//long long int a[7];
		//char cpu[4];
		//memset(cpu, '\0', sizeof(cpu));

		FILE *fp = fopen("/proc/stat", "r");
		//fscanf(fp, "%s %Ld %Ld %Ld %Ld %Ld %Ld %Ld", cpu, &a[0], &a[1], &a[2], &a[3], &a[4], &a[5], &a[6]);
		//printf("CPU ultilization:\n %s %Ld %Ld %Ld %Ld %Ld %Ld %Ld\n\n", cpu, a[0], a[1], a[2], a[3], a[4], a[5], a[6]);
		char stat_buffer[3*MSG_BUF_LEN];
		memset(stat_buffer, '\0', sizeof(stat_buffer));

		char cpu_util_str[] = "\n\nCPU Utilization: \n";
		strcpy(stat_buffer, cpu_util_str);
		int count = 0;

		count += strlen(cpu_util_str);

		while (!feof(fp))
		{
			stat_buffer[count++] = fgetc(fp);
		}
		stat_buffer[count++] = '\n';
	
		write_to_log_file(stat_buffer, count-1, fp_ct2);
	
		fclose(fp);
		
		usleep(100000);
	}
}

void write_to_log_file(char *buffer, int len, FILE *fp)
{
	sem_wait(&g_sem_lfile_write);
	
	fwrite(buffer, sizeof(char), len, fp);

	sem_post(&g_sem_lfile_write);
}
