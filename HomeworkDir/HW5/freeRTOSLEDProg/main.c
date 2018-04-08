#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "utils/uartstdio.h"

// TivaWare includes
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.c"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"

#include "inc/hw_memmap.h"

// FreeRTOS includes
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

#include "main.h"

xTimerHandle xLED2HzTaskTimer, xLED4HzTaskTimer;
BaseType_t xTimer1Start, xTimer2Start;
SemaphoreHandle_t xSemaphore;

uint32_t led1_status = 0;
uint32_t led2_status = 0;

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

void vLED2HzTimerCallbackFunc(TimerHandle_t xTimer)
{
    led1_status ^= GPIO_PIN_0;

    /* Turn on LED */
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, led1_status);
}

void vLED4HzTimerCallbackFunc(TimerHandle_t xTimer)
{
    led2_status ^= GPIO_PIN_1;

    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, led2_status);
}

void xUsrLED2HzTask(void * pvParameters)
{
    uint8_t ui8_msg_str[128];
    memset((char *)ui8_msg_str, '\0', sizeof(ui8_msg_str));

    /* Create a timer that expires every 500 msecs (2 Hz) */
    xLED2HzTaskTimer = xTimerCreate("LED2HzTimer",  /* Name of the timer */
                           500, /* timer period in ticks */
                           pdTRUE,  /* timer will reload itself when it expires */
                           (void *) 1, /* timer ID */
                           vLED2HzTimerCallbackFunc /* timer callback function */
                           );

    if (xLED2HzTaskTimer != NULL)
    {
        sprintf((char *)ui8_msg_str, "2Hz user led task timer created successfully\r\n");
        UARTSend(ui8_msg_str, strlen(ui8_msg_str));

        /* Configure GPIO to blink user LED D1 at 2Hz rate */
        /* Enable the GPIO port used for user LED 1 */
        ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);

        /* Enable the GPIO pins for user LED 1 */
        ROM_GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);
        MAP_GPIOPadConfigSet(GPIO_PORTN_BASE, GPIO_PIN_0,
                             GPIO_STRENGTH_12MA, GPIO_PIN_TYPE_STD);

        xTimer1Start = xTimerStart(xLED2HzTaskTimer, 0);
        if ( xTimer1Start == pdFALSE )
        {
            sprintf((char *)ui8_msg_str, "2Hz user led task timer start failed\r\n");
            UARTSend(ui8_msg_str, strlen(ui8_msg_str));
        }
    }
    else
    {
        sprintf((char *)ui8_msg_str, "2Hz user led task timer creation failed\r\n");
        UARTSend(ui8_msg_str, strlen(ui8_msg_str));
    }

    while(1)
    {

    }
}

void xUsrLED4HzTask(void * pvParameters)
{
    uint8_t ui8_msg_str[128];
    memset((char *)ui8_msg_str, '\0', sizeof(ui8_msg_str));

    /* Create a timer that expires every 250 msecs (4 Hz) */
    xLED4HzTaskTimer = xTimerCreate("LED4HzTimer",  /* Name of the timer */
                                    250, /* timer period in ticks */
                                    pdTRUE,  /* timer will reload itself when it expires */
                                    (void *) 1, /* timer ID */
                                    vLED4HzTimerCallbackFunc /* timer callback function */
                                    );

    if (xLED4HzTaskTimer != NULL)
    {
        sprintf((char *)ui8_msg_str, "4Hz user led task timer created successfully\r\n");
        UARTSend(ui8_msg_str, strlen(ui8_msg_str));

        /* Configure GPIO to blink user LED D1 at 4Hz rate */
        /* Enable the GPIO port used for user LED 2 */
        ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);

        /* Enable the GPIO pins for user LED 2 */
        ROM_GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1);
        MAP_GPIOPadConfigSet(GPIO_PORTN_BASE, GPIO_PIN_1,
                             GPIO_STRENGTH_12MA, GPIO_PIN_TYPE_STD);

        xTimer2Start = xTimerStart(xLED4HzTaskTimer, 0);
        if ( xTimer2Start == pdFALSE )
        {
            sprintf((char *)ui8_msg_str, "4Hz user led task timer start failed\r\n");
            UARTSend(ui8_msg_str, strlen(ui8_msg_str));
        }
    }
    else
    {
        sprintf((char *)ui8_msg_str, "4Hz user led task timer creation failed\r\n");
        UARTSend(ui8_msg_str, strlen(ui8_msg_str));
    }

    while(1)
    {

    }
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

    /* Create a mutex for UART */
    xSemaphore = xSemaphoreCreateMutex();
}

int main(void)
{

    /* Here in the main function, we will create a simple two task program that will
     * blink two LEDs at different rates (LED1 - 2 Hz and LED2 - 4 Hz).
     * The functionality will use FreeRTOS's TimerAPI with callback functions to blink
     * the LEDs at the specified rate.
     *
     */

    /* ---------------------------------------------------------------------------------------------------------- */
    /* Configure system clock to be at the max (120 MHz) */
    gui32_sys_clock_rate = MAP_SysCtlClockFreqSet(SYSCTL_XTAL_25MHZ  /* Frequency of external crystal */
                                                  | SYSCTL_OSC_MAIN  /* Use an external crystal or oscillator */
                                                  | SYSCTL_USE_PLL   /* select the PLL output as the system clock */
                                                  | SYSCTL_CFG_VCO_480, /* set the PLL VCO output to 480-MHz */
                                                  SYSTEM_CLOCK /* requested processor frequency */);

    /* ---------------------------------------------------------------------------------------------------------- */

    /* Initialize the UART */
    UART_Init();

    /* Create the two tasks that will blink the LEDs at different rates */
    xTaskCreate(xUsrLED2HzTask, /* Task function */
                (const portCHAR *)"usrLED2HzTask", /* Task Name */
                configMINIMAL_STACK_SIZE, /* Task stack size */
                NULL, /* Parameters */
                1, /* Priority */
                NULL /* task handle */ );

    /* Create the two tasks that will blink the LEDs at different rates */
    xTaskCreate(xUsrLED4HzTask, /* Task function */
                (const portCHAR *)"usrLED4HzTask", /* Task Name */
                configMINIMAL_STACK_SIZE, /* Task stack size */
                NULL, /* Parameters */
                1, /* Priority */
                NULL /* task handle */ );

    vTaskStartScheduler();

    return 0;
}
