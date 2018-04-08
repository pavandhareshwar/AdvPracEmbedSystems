/******************************************************************************
* Author: Pavan Dhareshwar
* Date: 3/6/2018
* File: main.h
* Description:  This file serves as a header file containing the macros,
*               globals, structures/enumerations and function prototypes
*               for main.c file.
*
******************************************************************************/

#ifndef MAIN_H_
#define MAIN_H_

// System clock rate, 120 MHz
#define SYSTEM_CLOCK    120000000U
#define QUEUE_LEN       20

#define TOGGLE_LED      ( 1UL << 0UL )
#define LOG_STRING      ( 1UL << 1UL )

#define MSG_MAX_LEN     128

uint32_t gui32_sys_clock_rate;
uint32_t gui32_led_status;

#define SYSTEM_CLOCK    120000000U

struct qmsg
{
    uint8_t     ui8_message[256];
    uint32_t    ui32_length;
};


/**
 *  @brief UART init
 *
 *  This function will perform the system level initialization of UART
 *
 *  @param      void
 *
 *  @return     void
*/
void UART_Init(void);

/**
 *  @brief Create queue and primary tasks
 *
 *  This function will create two things :
 *  1.  A queue that will be used for communication between the third task
 *      handling LEDs and UART and the second task that notifies the third
 *      task of the LOG_STRING event.
 *  2. The primary tasks created are:
 *      a.  Task1 (2 Hz task) that will generate TOGGLE_LED event notification
 *          to task 3.
 *      b.  Task2 (4 Hz task) that will generate LOG_STRING event notification
 *          and pass on the string to be printed to UART to task 3.
 *      c.  Task3 that will wait on task notifications from task 1 and 2 and
 *          control the LED and UART prints.
 *
 *  @param      void
 *
 *  @return     void
*/
void createQueueAndTasks(void);

/**
 *  @brief Create primary tasks
 *
 *  This function will create the queue that will be used for communication
 *  between the third task handling LEDs and UART and the second task that
 *  notifies the third task of the LOG_STRING event.
 *
 *  @param      void
 *
 *  @return     void
*/
void createTasks(void);

/**
 *  @brief The LED event notification producer task
 *
 *  This function will serve as the entry point for the led event notification
 *  producer task
 *
 *  @param      pvParameters:   pointer to parameter list
 *
 *  @return     void
*/
void vToggleLEDEventProdTask(void * pvParameters);

/**
 *  @brief The LED event notification producer task
 *
 *  This function will serve as the entry point for the led event notification
 *  producer task
 *
 *  @param      pvParameters:   pointer to parameter list
 *
 *  @return     void
*/
void vLogStringEventProdTask(void * pvParameters);

/**
 *  @brief The log string event notification producer task
 *
 *  This function will serve as the entry point for the log string event
 *  notification producer task
 *
 *  @param      pvParameters:   pointer to parameter list
 *
 *  @return     void
*/
void vSignalHandlerTask(void * pvParameters);

/**
 *  @brief Process the toggle LED event
 *
 *  This function will handling processing of the LED toggle event received
 *  by the third task
 *
 *  @param      void
 *
 *  @return     void
*/
void prvProcessToggleLedEvent(void);

/**
 *  @brief Process the log string event
 *
 *  This function will handling processing of the log string event received
 *  by the third task
 *
 *  @param      void
 *
 *  @return     void
*/
void prvProcessLogStringEvent(void);

/**
 *  @brief Process the event notifications
 *
 *  This function will handling processing of the log string event and LED
 *  toggle event notifications received by the third task
 *
 *  @param      ul_events_to_process : notification value of the recipient task
 *
 *  @return     void
*/
void ProcessEvents(unsigned long ul_events_to_process);

/**
 *  @brief Send a string to UART
 *
 *  This function will send a string to be printed to the UART
 *
 *  @param  pui8_str        : pointer to the string to be printed
 *  @param  ui32_str_len    : length of the string
 *
 *  @return void
*/
void UARTSend(uint8_t *pui8_str, uint32_t ui32_str_len);

#endif /* MAIN_H_ */
