#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#include "ipc_comm.h"

#define SHARED_MEM_BUF_SIZE 	(1024)

int main(void)
{
	pid_t child_pid;

	child_pid = fork();

	if (child_pid == 0)
	{
		/* 	Child process */
		
		/* Create the shared memory */
		key_t ipc_shm_key;
		int ipc_shmid;
		struct ipc_msg_struct recv_msg_struct;
		struct ipc_msg_struct *ptr_msg_struct = &recv_msg_struct;

		ipc_shm_key = ftok("/dev/zero", 'R');
		if (ipc_shm_key== -1)
		{
			perror("ftok failed");
			exit(1);
		}

		ipc_shmid = shmget(ipc_shm_key, SHARED_MEM_BUF_SIZE, 0644);
		if (ipc_shmid == -1)
		{
			perror("shmget failed");
			exit(1);
		}

		/* 	Attach the shared memory segment to parent process's address 
			space
		*/
		ptr_msg_struct = shmat(ipc_shmid, (void *)0, 0);
		if (ptr_msg_struct == (void *)-1)
		{
			perror("shmat failed");
			exit(1);
		}	

		/* Read data to shared memory */
		char read_buffer[1024];
		memset(read_buffer, '\0', sizeof(read_buffer));
		strncpy(read_buffer, ptr_msg_struct->msg.message, ptr_msg_struct->msg.msg_len);

		printf("Message received in child process: %s\n", read_buffer);
		printf("LED status: %s\n", (ptr_msg_struct->usr_led_ctrl == LED_ON) ? "ON": "OFF");

		
		/* Write data to shared memory from child process */
		char send_msg[] = "IPC communication using shared memory";
		strcpy(ptr_msg_struct->msg.message, send_msg);
		ptr_msg_struct->msg.msg_len = strlen(send_msg);
		ptr_msg_struct->usr_led_ctrl = LED_OFF;


		/* Detach shared memory segment */
		if (shmdt(ptr_msg_struct) == -1)
		{
			perror("shmdt failed");
			exit(1);
		}

		exit(0);
	}
	else if (child_pid > 0)
	{
		/* 	Parent process */
		/* 	Parent process will request for shared memory and write some
			data to it. The child process will then access the shared memory
			and read the data written by the parent process 
		*/
		
		/* Create the shared memory */
		key_t ipc_shm_key;
		int ipc_shmid;
		struct ipc_msg_struct sender_msg_struct;
		struct ipc_msg_struct *ptr_msg_struct = &sender_msg_struct;

		ipc_shm_key = ftok("/dev/zero", 'R');
		if (ipc_shm_key== -1)
		{
			perror("ftok failed");
			exit(1);
		}

		ipc_shmid = shmget(ipc_shm_key, SHARED_MEM_BUF_SIZE, 0644 | IPC_CREAT);
		if (ipc_shmid == -1)
		{
			perror("shmget failed");
			exit(1);
		}

		/* 	Attach the shared memory segment to parent process's address 
			space
		*/
		ptr_msg_struct = shmat(ipc_shmid, (void *)0, 0);
		if (ptr_msg_struct == (void *)-1)
		{
			perror("shmat failed");
			exit(1);
		}	

		/* Write data to shared memory */
		char send_msg[] = "IPC communication using shared memory";
		strcpy(ptr_msg_struct->msg.message, send_msg);
		ptr_msg_struct->msg.msg_len = strlen(send_msg);
		ptr_msg_struct->usr_led_ctrl = LED_ON;

		sleep(3);
		
		/* Read data to shared memory */
		char read_buffer[1024];
		memset(read_buffer, '\0', sizeof(read_buffer));
		strncpy(read_buffer, ptr_msg_struct->msg.message, ptr_msg_struct->msg.msg_len);

		printf("Message received in parent process: %s\n", read_buffer);
		printf("LED status: %s\n", (ptr_msg_struct->usr_led_ctrl == LED_ON) ? "ON": "OFF");

		/* Detach shared memory segment */
		if (shmdt(ptr_msg_struct) == -1)
		{
			perror("shmdt failed");
			exit(1);
		}

		sleep(2);
		
		if (shmctl(ipc_shmid, IPC_RMID, NULL) == -1)
		{
			perror("shmctl failed");
			exit(1);
		}
	}
	else 
	{
		printf("%s:%d :: Fork failed\n", __func__, __LINE__);
	}

	return 0;
}
