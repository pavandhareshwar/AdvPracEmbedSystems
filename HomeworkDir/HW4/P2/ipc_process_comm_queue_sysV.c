#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>

#include "ipc_comm.h"

/* Structure for message queue */
typedef struct msg
{
	long message_type;
	char message[256];
}qmsg;

int main(void)
{
	pid_t child_pid;

	child_pid = fork();

	if (child_pid == 0)
	{
		/* Child process */
		key_t ipc_queue_key;
		int ipc_queue_msg_id;
		
		qmsg qmsg_recv_buffer = {0};

		ipc_queue_key = ftok("/dev/urandom", 'R');
		if (ipc_queue_key == -1)
		{	
			perror("ftok failed");
			exit(1);
		}	
		
		/* Create a message queue */
		ipc_queue_msg_id = msgget(ipc_queue_key, 0766);
		if (ipc_queue_msg_id == -1)
		{
			perror("mssget failed");
			exit(1);
		}

		/* Send message to message queue */
		msgrcv(ipc_queue_msg_id, &qmsg_recv_buffer, sizeof(qmsg), 1, 0);
	
		printf("Message received in child process: %s\n", qmsg_recv_buffer.message);

		/* Destroy message queue */
		if (msgctl(ipc_queue_msg_id, IPC_RMID, NULL))
		{
			perror("msgctl failed");
		}

		exit(0);	
			
	}
	else if (child_pid > 0)
	{
		/* Parent process */
		key_t ipc_queue_key;
		int ipc_queue_msg_id;

		ipc_queue_key = ftok("/dev/urandom", 'R');
		if (ipc_queue_key == -1)
		{	
			perror("ftok failed");
			exit(1);
		}	

		/* Create a message queue */
		ipc_queue_msg_id = msgget(ipc_queue_key, 0766| IPC_CREAT);
		if (ipc_queue_msg_id == -1)
		{
			perror("mssget failed");
			exit(1);
		}

		qmsg qmsg_send_buffer = {0};

		qmsg_send_buffer.message_type = 1;

		strcpy(qmsg_send_buffer.message, "IPC communication using queues");

		/* Send message to message queue */
		msgsnd(ipc_queue_msg_id, &qmsg_send_buffer, sizeof(qmsg), 0);
	
	}
	else 
	{
		perror("fork failed");
	}

	return 0;
}
