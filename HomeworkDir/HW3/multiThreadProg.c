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

    char log_file_name[FNAME_BUF_LEN];
    memset(log_file_name, '\0', sizeof(log_file_name));
	strncpy(log_file_name, argv[1], strlen(argv[1]));

    printf("Log File Name: %s\n", log_file_name);

    /* Check if the file exists, if not create a file with R+W permissions */
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

    x_thread_info thread_info;
    memset(&thread_info, '\0', sizeof(x_thread_info));

    strcpy(thread_info.log_file_name, log_file_name);

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
	
	//signal(SIGUSR1, sig_handler_ct1);
	signal(SIGUSR2, sig_handler_ct1);
	signal(SIGUSR1, sig_handler_ct2);
	//signal(SIGUSR2, sig_handler_ct2);

	sleep(2);

	pthread_kill(t_child_1_id, SIGUSR1);
	pthread_kill(t_child_2_id, SIGUSR2);

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

		sprintf(buffer, "[Child 1] Thread exit\n");
		write_to_log_file(buffer, strlen(buffer), fp_ct1);

		if (fp_ct1)
			fclose(fp_ct1);
	
		pthread_exit(NULL);
	}
}

void sig_handler_ct2(int sig)
{
	char buffer[MSG_BUF_LEN];
	memset(buffer, '\0', sizeof(buffer));
	
	if (sig == SIGUSR1 || sig == SIGUSR2)
	{
		printf("Caught signal in child thread 2: %d\n", sig);
		
		sprintf(buffer, "[Child 2] Thread exit\n");
		write_to_log_file(buffer, strlen(buffer), fp_ct2);

		if (fp_ct2)	
			fclose(fp_ct2);
	
		pthread_exit(NULL);
	}
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

	/* Time when the thread started */
	time_t start_time = time(NULL); 
	char *time_str = ctime(&start_time);

	sprintf(buffer, "[Child 1] Thread start at %s (POSIX tid: %u, Linux tid: %d)\n", time_str, (unsigned int)posix_tid, linux_tid);
	write_to_log_file(buffer, strlen(buffer), fp_ct1);

	while(1)
	{
		printf("In child 1\n");
		sleep(1);
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

	/* POSIX and linux thread id information */
	pthread_t posix_tid = pthread_self();
	pid_t linux_tid = syscall( __NR_gettid );

	/* Time when the thread started */
	time_t start_time = time(NULL); 
	char *time_str = ctime(&start_time);

	sprintf(buffer, "[Child 2] Thread start at %s (POSIX tid: %u, Linux tid: %d)\n", time_str, (unsigned int)posix_tid, linux_tid);
	write_to_log_file(buffer, strlen(buffer), fp_ct2);

	while(1)
	{
		printf("In child 2\n");
		sleep(1);
	}
}

void write_to_log_file(char *buffer, int len, FILE *fp)
{
	sem_wait(&g_sem_lfile_write);
	
	fwrite(buffer, sizeof(char), len, fp);

	sem_post(&g_sem_lfile_write);
}
