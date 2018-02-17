#ifndef _MULTI_THREAD_H_
#define _MULTI_THREAD_H_

/*--------------------------- MACROS ------------------------*/
#define MSG_BUF_LEN			(1024)
#define FNAME_BUF_LEN		(256)

/*--------------------------- MACROS ------------------------*/

/*--------------------------- GLOBALS -----------------------*/

pthread_t t_master_id, t_child_1_id, t_child_2_id;

sem_t g_sem_lfile_write;

FILE *fp_master = NULL;
FILE *fp_ct1 = NULL;
FILE *fp_ct2 = NULL;

/*--------------------------- GLOBALS -----------------------*/

/*------------------ STRUCTURES/ENUMERATIONS-----------------*/

typedef struct _x_thread_info
{
    char log_file_name[FNAME_BUF_LEN];
}x_thread_info; 

/*------------------ STRUCTURES/ENUMERATIONS-----------------*/

/*------------------ FUNCTION PROTOTYPES --------------------*/
/**
 *  @brief Creates all the threads
 *  
 *  This function creates the master and the two child threads
 *  that will be used in this program. Each thread created will
 *  be passed as its argument a thread_info structure containing 
 *  the log file.

 *  @param thread_info : thread_info structure 
 *
 *  @return 0  : if thread creation is a success
            -1 : if thread creation fails
*/

int create_threads(x_thread_info thread_info);

void *mt_func(void *arg);

void *ct1_func(void *arg);

void *ct2_func(void *arg);

void write_to_log_file(char *buffer, int len, FILE *fp);

void sig_handler_ct1(int sig);

void sig_handler_ct2(int sig);

/*------------------ FUNCTION PROTOTYPES --------------------*/

#endif // #ifndef _MULTI_THREAD_H_ 
