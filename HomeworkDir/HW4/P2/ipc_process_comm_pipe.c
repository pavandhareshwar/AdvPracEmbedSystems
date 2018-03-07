/*****************************************************************
* Author: Pavan Dhareshwar
* Date: 3/6/2018
* File: ipc_process_comm_pipe.c
* Description: This source file implements the IPC communication 
*              mechanism between a parent and a child process 
*              created using fork system call using pipes.
******************************************************************/


#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "ipc_comm.h"

#define MSG_BUFF_LEN 			256

int main(void)
{

	int pipe_fds[2];

	pid_t child_pid;
	
	const char msg[] = "IPC communication using pipes";
	char msg_buffer[MSG_BUFF_LEN];
	memset(msg_buffer, '\0', sizeof(msg_buffer));
	
	pipe(pipe_fds);

	// pipe_fds[0] - used for reading
	// pipe_fds[1] - used for writing
	
	child_pid = fork();

	if (child_pid == 0)
	{
		/* Child process */
		//write(pipe_fds[1], msg, (strlen(msg)+1));
		
		//size_t num_read_bytes = read(pipe_fds[0], msg_buffer, sizeof(msg_buffer));
		struct ipc_msg_struct receiver_msg_struct = {0};
		size_t num_read_bytes = read(pipe_fds[0], &receiver_msg_struct, sizeof(struct ipc_msg_struct));
		printf("Received message in child: %s\n", receiver_msg_struct.msg.message);
		printf("LED info: %s\n", (receiver_msg_struct.usr_led_ctrl == LED_ON) ? "ON": "OFF");
		
		struct ipc_msg_struct sender_msg_struct = {0};
		sender_msg_struct.usr_led_ctrl = LED_OFF;
		strcpy(sender_msg_struct.msg.message, msg);
		sender_msg_struct.msg.msg_len = strlen(msg);
		
		write(pipe_fds[1], &sender_msg_struct, sizeof(struct ipc_msg_struct));
		printf("Message sent from child\n");
		
		exit(0);
		
	}
	else if (child_pid > 0)
	{
		//write(pipe_fds[1], msg, (strlen(msg)+1));
		
		struct ipc_msg_struct sender_msg_struct = {0};
		sender_msg_struct.usr_led_ctrl = LED_ON;
		strcpy(sender_msg_struct.msg.message, msg);
		sender_msg_struct.msg.msg_len = strlen(msg);
		
		write(pipe_fds[1], &sender_msg_struct, sizeof(struct ipc_msg_struct));

		printf("Message sent from parent\n");

		/* Wait for child to receive the message and send a message of its own */
		sleep(2);

		/* receive the message sent from child process */
		struct ipc_msg_struct receiver_msg_struct = {0};
		size_t num_read_bytes = read(pipe_fds[0], &receiver_msg_struct, sizeof(struct ipc_msg_struct));
		printf("Received message in parent: %s\n", receiver_msg_struct.msg.message);
		printf("LED info: %s\n", (receiver_msg_struct.usr_led_ctrl == LED_ON) ? "ON": "OFF");
		
	}
	else
	{
		printf("%s:%d :: fork failed\n", __func__, __LINE__);
	}

	return 0;
}
