#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>

#include <mqueue.h>

#include "ipc_comm.h"

#define QUEUE_MSG_SIZE				1024

int main(void)
{
	pid_t child_pid;

	struct mq_attr msg_queue_attr = { 	.mq_maxmsg = 5,
										.mq_msgsize = QUEUE_MSG_SIZE,
										.mq_flags = 0};

	mqd_t msg_queue;
	int msg_priority;
	
	child_pid = fork();

	if (child_pid == 0)
	{
		/* Child process */
		
		msg_priority = 1;
		msg_queue = mq_open("/testqueue", O_RDWR, S_IRWXU, &msg_queue_attr);
		if (msg_queue < 0)
		{
			perror("mq_open failed\n");
			return -1;
		}
		
		char recv_buffer[QUEUE_MSG_SIZE];
		memset(recv_buffer, '\0', sizeof(recv_buffer));

		int num_rcv_bytes = mq_receive(msg_queue, (char *)&recv_buffer, 
								QUEUE_MSG_SIZE, &msg_priority);

		if (num_rcv_bytes < 0)
		{
			perror("mq_receive failed\n");
			return -1;
		}
		else
		{
			printf("Message received: %s\n", ((struct ipc_msg_struct *)&recv_buffer)->msg.message);
			printf("LED status: %s\n", (((struct ipc_msg_struct *)&recv_buffer)->usr_led_ctrl == LED_ON) ? "on" : "off");	
		}
		
		struct ipc_msg_struct sender_msg;
	
		strcpy(sender_msg.msg.message, "IPC communication using queues");
		sender_msg.msg.msg_len = strlen("IPC communication using queues");
		sender_msg.usr_led_ctrl = LED_OFF;

		int num_sent_bytes = mq_send(msg_queue, (char *)&sender_msg,
								sizeof(sender_msg), msg_priority);
		if (num_sent_bytes < 0)
		{
			perror("mq_send failed\n");
			return -1;
		}
		else
		{
			printf("Message sent from child\n");
		}
		exit(0);	
			
	}
	else if (child_pid > 0)
	{
		/* Parent process */
		msg_priority = 1;
		msg_queue = mq_open("/testqueue", O_CREAT | O_RDWR, S_IRWXU, &msg_queue_attr);
		if (msg_queue < 0)
		{
			perror("mq_open failed\n");
			return -1;
		}

		struct ipc_msg_struct sender_msg;
		
		strcpy(sender_msg.msg.message, "IPC communication using queues");
		sender_msg.msg.msg_len = strlen("IPC communication using queues");
		sender_msg.usr_led_ctrl = LED_ON;

		int num_sent_bytes = mq_send(msg_queue, (char *)&sender_msg,
								sizeof(sender_msg), msg_priority);
		if (num_sent_bytes < 0)
		{
			perror("mq_send failed\n");
			return -1;
		}
		else
		{
			printf("Message sent from parent\n");
		}

		sleep(5);

		char recv_buffer[QUEUE_MSG_SIZE];
		memset(recv_buffer, '\0', sizeof(recv_buffer));

		int num_rcv_bytes = mq_receive(msg_queue, (char *)&recv_buffer, 
								QUEUE_MSG_SIZE, &msg_priority);

		if (num_rcv_bytes < 0)
		{
			perror("mq_receive failed\n");
			return -1;
		}
		else
		{
			printf("Message received: %s\n", ((struct ipc_msg_struct *)&recv_buffer)->msg.message);
			printf("LED status: %s\n", (((struct ipc_msg_struct *)&recv_buffer)->usr_led_ctrl == LED_ON) ? "on" : "off");	
		}
	
	}
	else 
	{
		perror("fork failed");
	}

	return 0;
}
