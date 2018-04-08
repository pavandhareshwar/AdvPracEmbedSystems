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
#include "driverlib/interrupt.h"
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

#include "main.h"

TaskHandle_t hdl_task1, hdl_task2, hdl_task3;
xTimerHandle xTimer1, xTimer2;
BaseType_t xTimer1Start, xTimer2Start;
QueueHandle_t xQueue = NULL;

#define QUEUE_LEN       20

#define TOGGLE_LED      ( 1UL << 0UL )
#define LOG_STRING      ( 1UL << 1UL )

#define SYSTEM_CLOCK    120000000U

struct qmsg
{
    uint8_t     ui8_message[256];
    uint32_t    ui32_length;
};

void UARTHandler(void)
{
    uint32_t ui32Status;

    //
    // Get the interrrupt status.
    //
    ui32Status = ROM_UARTIntStatus(UART0_BASE, true);

    //
    // Clear the asserted interrupts.
    //
    ROM_UARTIntClear(UART0_BASE, ui32Status);

    //
    // Loop while there are characters in the receive FIFO.
    //
    while(ROM_UARTCharsAvail(UART0_BASE))
    {
        //
        // Read the next character from the UART and write it back to the UART.
        //
        UARTCharPut(UART0_BASE,
                    ROM_UARTCharGetNonBlocking(UART0_BASE));
    }
}

void UARTSend(uint8_t *pui8_str, uint32_t ui32_str_len)
{
    while (ui32_str_len != 0)
    {
        /* Write a single character to UART */
        ROM_UARTCharPutNonBlocking(UART0_BASE, *pui8_str);
        ui32_str_len--;
        pui8_str++;
    }
}

void timer1CallbackFunc(TimerHandle_t pxTimer)
{
    //unsigned long ul_count = 1;

    while(1)
    {
        xTaskNotify(hdl_task3, TOGGLE_LED, eSetBits);
        //ul_count++;
        //SysCtlDelay(gui32_sys_clock_rate / (1000 * 3));
        vTaskDelay(1000/portTICK_PERIOD_MS); //wait for 2 seconds
    }
}

void timer2CallbackFunc(TimerHandle_t pxTimer)
{
    //unsigned long ul_count = 1;
    struct qmsg msg_to_send;
    unsigned char uc_message[] = "Hello\n";
    strcpy(msg_to_send.ui8_message, uc_message);
    msg_to_send.ui32_length = strlen(uc_message);

    if (xQueue != NULL)
    {
        while(1)
        {
            xQueueSend(xQueue, (void *)&msg_to_send, (TickType_t) 0);
            xTaskNotify(hdl_task3, LOG_STRING, eSetBits);
            //ul_count++;
            //SysCtlDelay(gui32_sys_clock_rate / (1000 * 3));
            vTaskDelay(2000/portTICK_PERIOD_MS); //wait for 4 seconds
        }
    }

}

void task1(void * pvParameters)
{
    /* Create a timer that expires every 1 second */

    /* Configure GPIO to blink user LED D1 at 2Hz rate */
    /* Enable the GPIO port used for user LED 1 */
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);

    /* Enable the GPIO pins for user LED 1 */
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1);
    MAP_GPIOPadConfigSet(GPIO_PORTN_BASE, GPIO_PIN_1,
                         GPIO_STRENGTH_12MA, GPIO_PIN_TYPE_STD);

    xTimer1 = xTimerCreate("Timer1",  /* Name of the timer */
                           100, /* timer period in ticks */
                           pdTRUE,  /* timer will reload itself when it expires */
                           (void *) 1, /* timer ID */
                           timer1CallbackFunc /* timer callback function */
                           );

#if 0
    if (xTimer1 != NULL)
    {
        xTimer1Start = xTimerStart(xTimer1, 0);
        if ( xTimer1Start == pdFALSE )
        {
            printf("Timer1 start failed\n");
        }
    }
    else
    {
        printf("Timer1 creation failed\n");
    }
#endif


    for (;;) { }

}

void task2(void * pvParameters)
{
    /* Create a timer that expires every 2 second */

    xTimer2 = xTimerCreate("Timer2",  /* Name of the timer */
                           200, /* timer period in ticks */
                           pdTRUE,  /* timer will reload itself when it expires */
                           (void *) 2, /* timer ID */
                           timer2CallbackFunc /* timer callback function */
                           );

    if (xTimer2 != NULL)
    {
        xTimer2Start = xTimerStart(xTimer2, 0);
        if ( xTimer2Start == pdFALSE )
        {
            printf("Timer2 start failed\n");
        }
    }
    else
    {
        printf("Timer2 creation failed\n");
    }


    for (;;) { }

}

void prvProcessToggleLedEvent(void)
{
    /* Turn on LED */
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, GPIO_PIN_1);

    /* Delay of 1 ms */
    /* TODO: Change this to 2 Hz delay */
    SysCtlDelay((gui32_sys_clock_rate / (1000 * 3))*2);

    /* Turn on LED */
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, 0);
}

void prvProcessLogStringEvent(void)
{
    //unsigned long ul_count;
    struct qmsg msg_rcvd;

    xQueueReceive(xQueue, &msg_rcvd, (TickType_t )(1000/portTICK_PERIOD_MS));
    //printf("Message received : %s\n", msg_rcvd.ui8_message);
    //UARTSend((uint8_t *)msg_rcvd.ui8_message, msg_rcvd.ui8_message);
    UARTSend((char *)msg_rcvd.ui8_message, 6);
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

void task3(void * pvParameters)
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

            //xQueueReceive(xQueue, ui8_message, ( TickType_t) 10);
            vTaskDelay(500/portTICK_PERIOD_MS); //wait for 500 ms
        }
    }
}

void createTasks(void)
{
    /* Create the two tasks that will blink the LEDs at different rates */
    xTaskCreate(task1, /* Task function */
                (const portCHAR *)"2HzTask", /* Task Name */
                configMINIMAL_STACK_SIZE, /* Task stack size */
                NULL, /* Parameters */
                1, /* Priority */
                &hdl_task1 /* task handle */ );

    /* Create the two tasks that will blink the LEDs at different rates */
    xTaskCreate(task2, /* Task function */
                (const portCHAR *)"4HzTask", /* Task Name */
                configMINIMAL_STACK_SIZE, /* Task stack size */
                NULL, /* Parameters */
                1, /* Priority */
                &hdl_task2 /* task handle */ );

    /* Create the two tasks that will blink the LEDs at different rates */
    xTaskCreate(task3, /* Task function */
                (const portCHAR *)"ThirdTask", /* Task Name */
                configMINIMAL_STACK_SIZE, /* Task stack size */
                NULL, /* Parameters */
                1, /* Priority */
                &hdl_task3 /* task handle */ );
}

void createQueueAndTasks(void)
{
    //xQueue = xQueueCreate(QUEUE_LEN, sizeof(unsigned long));
    xQueue = xQueueCreate(QUEUE_LEN, sizeof(struct qmsg));
    if(xQueue != NULL)
    {
        printf("Queue is created\n");
        SysCtlDelay(gui32_sys_clock_rate / (1000 * 3));

        createTasks();

        vTaskStartScheduler();
    }
    else
    {
        printf("Queue creation failed");
    }
}

void UART_Init(void)
{
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    ROM_UARTConfigSetExpClk(UART0_BASE, gui32_sys_clock_rate, 115200,
                            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                                    UART_CONFIG_PAR_NONE));

    ROM_IntEnable(INT_UART0);
    ROM_UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);

    UARTLoopbackEnable(UART0_BASE);
}

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
    //TaskHandle_t thdl_usr_led1, thdl_usr_led2;

    UART_Init();

    createQueueAndTasks();

    for(;;);

    return 0;
}
