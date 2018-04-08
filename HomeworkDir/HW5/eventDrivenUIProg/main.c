/******************************************************************************
* Author: Pavan Dhareshwar
* Date: 4/6/2018
* File: main.c
* Description:  This file defines the event driven UI functionality using two
*               tasks signalling a third new task that controls LED and UART.
*               The two tasks will notify the third task via task notifications,
*               which will be handled by the third task to either toggle an LED
*               or print a string using UART.
*
******************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "utils/uartstdio.h"

// TivaWare includes
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"

#include "utils/uartstdio.h"

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"

// FreeRTOS includes
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

#include "main.h"

TaskHandle_t hdl_toggle_led_event_task, hdl_log_str_event_task, hdl_sig_handler_task;
xTimerHandle xLEDEventTaskTimer, xLogStringEventTaskTimer;
BaseType_t xTimer1Start, xTimer2Start;
QueueHandle_t xQueue = NULL;

SemaphoreHandle_t xSemaphore = NULL;

int main(void)
{

    /* Here in the main function, we will create an event driven user interface consisting of
     * 3 tasks where the two user tasks from the previous program will signal the third task.
     *
     * The first user task will be responsible for managing one of the LEDs and will signal
     * TOGGLE_LED notification from its timer callback function
     *
     * The second user task will signal a LOG_STRING notification from its timer callback
     * function and pass the string to be printed on the terminal using a queue
     *
     * The third task will control both the UART and the LEDs and block wait on Tasknotify
     * signals from the other two tasks.
     */

    /* ---------------------------------------------------------------------------------------------------------- */
    /* Configure system clock to be at the max (120 MHz) */
    gui32_sys_clock_rate = MAP_SysCtlClockFreqSet(SYSCTL_XTAL_25MHZ  /* Frequency of external crystal */
                                                  | SYSCTL_OSC_MAIN  /* Use an external crystal or oscillator */
                                                  | SYSCTL_USE_PLL   /* select the PLL output as the system clock */
                                                  | SYSCTL_CFG_VCO_480, /* set the PLL VCO output to 480-MHz */
                                                  SYSTEM_CLOCK /* requested processor frequency */
                                                  );

    /* ---------------------------------------------------------------------------------------------------------- */

    gui32_led_status = 0;

    /* UART initialization */
    UART_Init();

    /* Create the queue and the primary tasks */
    createQueueAndTasks();

    for(;;);

    return 0;
}

void UART_Init(void)
{
    /* Enable the peripherals */
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    /* Configure GPIO PA0 and PA1 as UART pins */
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    /* Configure the UART : baud rate: 115200, 1 stop bit and no parity */
    ROM_UARTConfigSetExpClk(UART0_BASE, gui32_sys_clock_rate, 115200,
                            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                                    UART_CONFIG_PAR_NONE));

    /* Create a semaphore for UART */
    xSemaphore = xSemaphoreCreateMutex();

}

void createQueueAndTasks(void)
{
    uint8_t ui8_msg_str[MSG_MAX_LEN];
    memset((char *) ui8_msg_str, '\0', sizeof(ui8_msg_str));

    xQueue = xQueueCreate(QUEUE_LEN, sizeof(struct qmsg));
    if(xQueue != NULL)
    {
        sprintf((char *)ui8_msg_str, "Queue created successfully\r\n");
        UARTSend(ui8_msg_str, strlen(ui8_msg_str));

        createTasks();

        vTaskStartScheduler();
    }
    else
    {
        sprintf((char *)ui8_msg_str, "Queue creation failed\r\n");
        UARTSend(ui8_msg_str, strlen(ui8_msg_str));
    }
}

void createTasks(void)
{
    /* Create the two tasks that will blink the LEDs at different rates */
    xTaskCreate(vToggleLEDEventProdTask, /* Task function */
                (const portCHAR *)"ToggleLEDEventProdTask", /* Task Name */
                configMINIMAL_STACK_SIZE, /* Task stack size */
                NULL, /* Parameters */
                1, /* Priority */
                &hdl_toggle_led_event_task /* task handle */
                );

    /* Create the two tasks that will blink the LEDs at different rates */
    xTaskCreate(vLogStringEventProdTask, (const portCHAR *)"LogStringEventProdTask",
                configMINIMAL_STACK_SIZE, NULL,
                1, &hdl_log_str_event_task );

    /* Create the two tasks that will blink the LEDs at different rates */
    xTaskCreate(vSignalHandlerTask, (const portCHAR *)"SignalHandlerTask",
                configMINIMAL_STACK_SIZE, NULL,
                1, &hdl_sig_handler_task);
}

void vLEDEventTaskTimerCbFunc(TimerHandle_t xTimer)
{
    /* Task 1 will signal task3’s TOGGLE_LED event notification here
     * in the timer callback function */
    xTaskNotify(hdl_sig_handler_task, TOGGLE_LED, eSetBits);
}

void vLogStringEventTaskTimerCbFunc(TimerHandle_t xTimer)
{
    struct qmsg msg_to_send;
    unsigned char uc_message[MSG_MAX_LEN];

    TickType_t tick_count = xTaskGetTickCount();

    sprintf((char *)uc_message, "Task 2 tick count: %d\r\n", tick_count);
    strcpy(msg_to_send.ui8_message, uc_message);
    msg_to_send.ui32_length = strlen(uc_message);

    if (xQueue != NULL)
    {
        xQueueSend(xQueue, (void *)&msg_to_send, (TickType_t) 0);

        xTaskNotify(hdl_sig_handler_task, LOG_STRING, eSetBits);
    }
}

void vToggleLEDEventProdTask(void * pvParameters)
{
    uint8_t ui8_msg_str[MSG_MAX_LEN];
    memset(ui8_msg_str, '\0', sizeof(ui8_msg_str));

    /* -------------------------------------------------------------------------------- */
    /* Configure GPIO to blink user LED D1 at 2Hz rate */
    /* Enable the GPIO port used for user LED 1 */
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);

    /* Enable the GPIO pins for user LED 1 */
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);
    MAP_GPIOPadConfigSet(GPIO_PORTN_BASE, GPIO_PIN_0,
                         GPIO_STRENGTH_12MA, GPIO_PIN_TYPE_STD);
    /* -------------------------------------------------------------------------------- */

    /* Create a timer that expires every 500 msecs (2 Hz) */
    xLEDEventTaskTimer = xTimerCreate("LEDEventTaskTimer",  /* Name of the timer */
                                      500, /* timer period in ticks */
                                      pdTRUE,  /* timer will reload itself when it expires */
                                      (void *) 1, /* timer ID */
                                      vLEDEventTaskTimerCbFunc /* timer callback function */
                                       );

    if (xLEDEventTaskTimer != NULL)
    {
        sprintf((char *)ui8_msg_str, "LED Event Task Timer creation successfully\r\n");
        UARTSend(ui8_msg_str, strlen(ui8_msg_str));

        SysCtlDelay(gui32_sys_clock_rate / (1000 * 3));

        xTimer1Start = xTimerStart(xLEDEventTaskTimer, 0);
        if ( xTimer1Start == pdFALSE )
        {
            sprintf((char *)ui8_msg_str, "LED Event Task Timer start failed\r\n");
            UARTSend(ui8_msg_str, strlen(ui8_msg_str));
        }
    }
    else
    {
        sprintf((char *)ui8_msg_str, "LED Event Task Timer creation failed\r\n");
        UARTSend(ui8_msg_str, strlen(ui8_msg_str));
    }

    /* -------------------------------------------------------------------------------- */

    for (;;) { }

}

void vLogStringEventProdTask(void * pvParameters)
{
    uint8_t ui8_msg_str[MSG_MAX_LEN];
    memset(ui8_msg_str, '\0', sizeof(ui8_msg_str));

    /* -------------------------------------------------------------------------------- */

    /* Create a timer that expires every 250 msecs (4 Hz) */
    xLogStringEventTaskTimer = xTimerCreate("LogStringEventTaskTimer",  /* Name of the timer */
                                            250, /* timer period in ticks */
                                            pdTRUE,  /* timer will reload itself when it expires */
                                            (void *) 1, /* timer ID */
                                            vLogStringEventTaskTimerCbFunc /* timer callback function */
                                            );

    if (xLogStringEventTaskTimer != NULL)
    {
        sprintf((char *)ui8_msg_str, "Log String Event Task Timer created successfully\r\n");
        UARTSend(ui8_msg_str, strlen(ui8_msg_str));

        SysCtlDelay(gui32_sys_clock_rate / (1000 * 3));

        xTimer2Start = xTimerStart(xLogStringEventTaskTimer, 0);
        if ( xTimer2Start == pdFALSE )
        {
            sprintf((char *)ui8_msg_str, "Log String Event Task Timer start failed\r\n");
            UARTSend(ui8_msg_str, strlen(ui8_msg_str));
        }
    }
    else
    {
        sprintf((char *)ui8_msg_str, "Log String Event Task Timer creation failed\r\n");
        UARTSend(ui8_msg_str, strlen(ui8_msg_str));
    }
    /* -------------------------------------------------------------------------------- */

    for (;;) { }

}

void vSignalHandlerTask(void * pvParameters)
{
    uint32_t ul_notified_value;

    while (xQueue != NULL)
    {
        while (1)
        {
            xTaskNotifyWait( 0x00,      /* Don't clear any notification bits on entry. */
                             0xffffffff, /* Reset the notification value to 0 on exit. */
                             &ul_notified_value, /* Notified value pass out in
                                                          ulNotifiedValue. */
                             portMAX_DELAY );  /* Block indefinitely. */

            ProcessEvents(ul_notified_value);

            vTaskDelay(500/portTICK_PERIOD_MS); //wait for 500 ms
        }
    }
}

void prvProcessToggleLedEvent(void)
{
    gui32_led_status ^= 0x1;

    /* Toggle the LED */
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, gui32_led_status);
}

void prvProcessLogStringEvent(void)
{
    struct qmsg x_msg_rcvd;
    uint8_t ui8_msg_str[MSG_MAX_LEN];

    if (xQueue != NULL)
    {
        xQueueReceive(xQueue, &x_msg_rcvd, (TickType_t )(1000/portTICK_PERIOD_MS));

        strcpy((char *)ui8_msg_str, (char *)x_msg_rcvd.ui8_message);

        UARTSend(ui8_msg_str, strlen(ui8_msg_str));
    }
}

void ProcessEvents(unsigned long ul_events_to_process)
{
    if( ( ul_events_to_process & TOGGLE_LED ) ==  TOGGLE_LED)
    {
        ul_events_to_process ^= TOGGLE_LED;
        prvProcessToggleLedEvent();
    }

    if( ( ul_events_to_process & LOG_STRING ) == LOG_STRING )
    {
        ul_events_to_process ^= LOG_STRING;
        prvProcessLogStringEvent();
    }
}

void UARTSend(uint8_t *pui8_str, uint32_t ui32_str_len)
{
    if( xSemaphoreTake( xSemaphore, ( TickType_t ) 10 ) == pdTRUE )
    {
        while (ui32_str_len != 0)
        {
            /* Write a single character to UART */
            UARTCharPut(UART0_BASE, *pui8_str);
            ui32_str_len--;
            pui8_str++;
        }

        xSemaphoreGive( xSemaphore );
    }
}
