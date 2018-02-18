#ifndef _MULTI_THREAD_H_
#define _MULTI_THREAD_H_

/*----------------------------------- MACROS --------------------------------*/

#define MSG_BUF_LEN			(1024)
#define FNAME_BUF_LEN		(256)

#define INP_TXT_FILE		"Valentinesday.txt"
#define REP_COUNT			3

/*----------------------------------- MACROS --------------------------------*/

/*---------------------------------- GLOBALS --------------------------------*/

/* 	Thread handles for 3 threads */
pthread_t t_master_id, t_child_1_id, t_child_2_id;

/* 	Semaphore used for mutually exclusive access of log file */
sem_t g_sem_lfile_write;

/* 	File handles for the log file each owned by 3 threads */
FILE *fp_master = NULL;
FILE *fp_ct1 = NULL;
FILE *fp_ct2 = NULL;

/*---------------------------------- GLOBALS --------------------------------*/

/*---------------------------- STRUCTURES/ENUMERATIONS ----------------------*/

/*	Thread info structure to pass arguments to the threads */
typedef struct _x_thread_info
{
    char log_file_name[FNAME_BUF_LEN];
}x_thread_info; 

/* 	Linked list node used to store character, its count and pointer to next 
	node
*/
struct node_t 
{
	int ch;
	int count;
	struct node_t *next;
};

/*---------------------------- STRUCTURES/ENUMERATIONS ----------------------*/

/*---------------------------- FUNCTION PROTOTYPES --------------------------*/
/**
 *  @brief Creates all the threads
 *  
 *  This function creates the master and the two child threads that will be 
 *	used in this program. Each thread created will be passed as its argument 
 * 	a thread_info structure containing the log file.

 *  @param thread_info : thread_info structure 
 *
 *  @return 0  : if thread creation is a success
            -1 : if thread creation fails
*/

int create_threads(x_thread_info thread_info);

/**
 *  @brief Start routine for master thread
 *  
 *  This function acts as the start routine for the master thread and is invoked 
 *	when the master thread is created. The function is executed and return from 
 *	from this function or invocation of pthread_exit marks the termination of 
 *	the thread. 

 *  @param arg : argument passed to mt_func
 *
 *  @return void
*/
void *mt_func(void *arg);

/**
 *  @brief Start routine for child thread 1
 *  
 *  This function acts as the start routine for child thread 1 and is invoked 
 *	when child thread 1 is created. The function is executed and return from 
 *	from this function or invocation of pthread_exit marks the termination of 
 *	the thread. 

 *  @param arg : argument passed to ct1_func
 *
 *  @return void
*/
void *ct1_func(void *arg);

/**
 *  @brief Start routine for child thread 2
 *  
 *  This function acts as the start routine for child thread 2 and is invoked 
 *	when child thread 2 is created. The function is executed and return from 
 *	from this function or invocation of pthread_exit marks the termination of 
 *	the thread. 

 *  @param arg : argument passed to ct2_func
 *
 *  @return void
*/
void *ct2_func(void *arg);

/**
 *  @brief Log file write function
 *  
 *  This function provides synchronized write access to the log file by the 
 *	master and 2 child threads. 

 *  @param buffer: 	pointer to buffer holding the message to be written to 
 *					log file
 *	@param len: 	message length
 *	@param fp:		log file pointer
 *
 *  @return void
*/
void write_to_log_file(char *buffer, int len, FILE *fp);

/**
 *  @brief Signal handler for child thread 1
 *  
 *  The signal handler defined for handling SIGUSR1 and SIGUSR2 signal
 *	delivered to child thread 1.

 *	@param sig: 	signal number
 *
 *  @return void
*/
void sig_handler_ct1(int sig);

/**
 *  @brief Signal handler for child thread 2
 *  
 *  The signal handler defined for handling SIGUSR1 and SIGUSR2 signal
 *	delivered to child thread 2.

 *	@param sig: 	signal number
 *
 *  @return void
*/
void sig_handler_ct2(int sig);

/**
 *  @brief Helper function to convert to lower case character
 *  
 *  This function helps with the conversion of upper case character to
 *	lower case.

 *	@param ch: 		upper/lower case input character
 *
 *  @return char:	converted lower case character or original lower
					case character
*/
char to_lower(char ch);

/**
 *  @brief Function to add a node to the linked list
 *  
 *  This function adds a node that will hold the character specified 
 * 	by @param c if such a node doesn't exist, or increments the count
 *	held by the previously created node holding the character.

 *	@param head:	Pointer to head of the linked list
 *	@param c:		Character to be added to linked list
 *
 *  @return void
*/
void add_node_to_list(struct node_t **head, int c);

/**
 *  @brief Function to print the nodes of the linked list
 *  
 *  This function will print the characters and the count of the character
 *	held by the nodes of the linked list. 

 *	@param head:	Head of the linked list
 *
 *  @return void
*/
void printList(struct node_t *head);

/**
 *  @brief Function to create a linked list of characters and its count
 *  
 *  This function creates a linked list with nodes holding information on
 * 	characters and its count present in a file specified by macro INP_TXT_FILE

 *	@param head:	Pointer to head of the linked list
 *
 *  @return void
*/
void create_list(struct node_t **head);

/**
 *  @brief Function to find nodes with specified number of occurrences
 *  
 *  This function is used to traverse through the linked list specified by 
 *	@param head and find nodes that contains #REP_COUNT number of character
 * 	occurrences recorded by count member of the linked list node.

 *	@param head:	Head of the linked list
 *
 *  @return void
*/
void find_nodes_with_mult_occurrence(struct node_t *head);

/*---------------------------- FUNCTION PROTOTYPES --------------------------*/

#endif // #ifndef _MULTI_THREAD_H_ 
