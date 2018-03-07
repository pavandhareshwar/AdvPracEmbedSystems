#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "ipc_comm.h"

#define SERVER_PORT_NUM 			8080
#define SERVER_LISTEN_QUEUE_SIZE	5

#define IPC_MSG_SIZE        256
#define BUFF_SIZE			1024

int main(void)
{

	pid_t child_pid;
	child_pid = fork();
	if (child_pid == 0)
	{
		/* Child Process */
		/* Child process will be made client */
		
		int client_sock;
		struct sockaddr_in serv_addr;
		char buffer[BUFF_SIZE];
		memset(buffer, '\0', sizeof(buffer));
		if ((client_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		{
				printf("\n Socket creation error \n");
				return -1;
		}

		memset(&serv_addr, '0', sizeof(serv_addr));

		serv_addr.sin_family = AF_INET;
		serv_addr.sin_port = htons(SERVER_PORT_NUM);

		// Convert IPv4 and IPv6 addresses from text to binary form
		if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) 
		{
				printf("\nInvalid address/ Address not supported \n");
				return -1;
		}

		if (connect(client_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
		{
				printf("\nConnection Failed \n");
				return -1;
		}
		
		char send_msg[] = "IPC communication using sockets";
		//size_t sent_bytes = send(client_sock , send_msg , strlen(send_msg) , 0 );
		struct ipc_msg_struct sender_msg_struct = {0};
		strcpy(sender_msg_struct.msg.message, send_msg);
		sender_msg_struct.msg.msg_len = strlen(send_msg);
		sender_msg_struct.usr_led_ctrl = LED_OFF;	
		size_t sent_bytes = send(client_sock , &sender_msg_struct , sizeof(struct ipc_msg_struct) , 0 );

		sleep(2);

		/* Receiving message from parent process */

		struct ipc_msg_struct receiver_msg_struct = {0};
		size_t num_read_bytes = read(client_sock, &receiver_msg_struct, sizeof(struct ipc_msg_struct));
		printf("Message received in child : %s\n", receiver_msg_struct.msg.message);
		printf("LED status : %s\n", (receiver_msg_struct.usr_led_ctrl == 0) ? "ON": "OFF");
		exit(0);
		
	}
	else
	{
		/* Parent process */
		int server_fd;
		struct sockaddr_in server_address;
		int serv_addr_len = sizeof(server_address);
		char buffer[BUFF_SIZE];

		memset(buffer, '\0', sizeof(buffer));


		// Creating a socket
		if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
		{
			perror("socket failed");
			exit(EXIT_FAILURE);
		}	

		server_address.sin_family = AF_INET;
		server_address.sin_addr.s_addr = INADDR_ANY;
		server_address.sin_port = htons(SERVER_PORT_NUM);

		if (bind(server_fd, (struct sockaddr *)&server_address, 
								sizeof(server_address))<0)
		{
			perror("bind failed");
			exit(EXIT_FAILURE);
		}

		if (listen(server_fd, SERVER_LISTEN_QUEUE_SIZE) < 0)
		{
			perror("listen");
			exit(EXIT_FAILURE);
		}

		int accept_conn_id;
		if ((accept_conn_id = accept(server_fd, (struct sockaddr *)&server_address, 
                       (socklen_t*)&serv_addr_len)) < 0)
		{
        	perror("accept");
        	exit(EXIT_FAILURE);
    	}

		//size_t num_read_bytes = read(accept_conn_id, buffer, sizeof(buffer));
		struct ipc_msg_struct receiver_msg_struct = {0};
		size_t num_read_bytes = read(accept_conn_id, &receiver_msg_struct, sizeof(struct ipc_msg_struct));
		printf("Message received in parent : %s\n", receiver_msg_struct.msg.message);
		printf("LED status : %s\n", (receiver_msg_struct.usr_led_ctrl == 0) ? "ON": "OFF");
	
		
		/* Sending message from parent */
		char send_msg[] = "IPC communication using sockets";
		//size_t sent_bytes = send(client_sock , send_msg , strlen(send_msg) , 0 );
		struct ipc_msg_struct sender_msg_struct = {0};
		strcpy(sender_msg_struct.msg.message, send_msg);
		sender_msg_struct.msg.msg_len = strlen(send_msg);
		sender_msg_struct.usr_led_ctrl = LED_ON;	
		size_t sent_bytes = send(accept_conn_id, &sender_msg_struct , sizeof(struct ipc_msg_struct) , 0 );
	}	
	
	
	return 0;	
}
