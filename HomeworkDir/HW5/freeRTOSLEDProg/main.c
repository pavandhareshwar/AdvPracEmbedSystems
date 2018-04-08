#include <stdint.h>
#include <stdbool.h>

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

#include "inc/hw_memmap.h"

// FreeRTOS includes
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

#include "main.h"

xTimerHandle xTimer1, xTimer2;
BaseType_t xTimer1Start, xTimer2Start;

void timer1CallbackFunc(TimerHandle_t pxTimer)
{
    /* Turn on LED */
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, GPIO_PIN_1);

    /* Delay of 1 ms */
    /* TODO: Change this to 2 Hz delay */
    SysCtlDelay(gui32_sys_clock_rate / (1000 * 3));

    /* Turn on LED */
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, 0);
}

void timer2CallbackFunc(TimerHandle_t pxTimer)
{

    /* Turn on LED */
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_2, GPIO_PIN_2);

    /* Delay of 2 ms */
    /* TODO: Change this to 2 Hz delay */
    SysCtlDelay(2*gui32_sys_clock_rate / (1000 * 3));

    /* Turn on LED */
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_2, 0);
}

void usrLED1Task(void * pvParameters)
{
    /* Create a timer that expires every 1 second */

    xTimer1 = xTimerCreate("Timer1",  /* Name of the timer */
                           100, /* timer period in ticks */
                           pdTRUE,  /* timer will reload itself when it expires */
                           (void *) 1, /* timer ID */
                           timer1CallbackFunc /* timer callback function */
                           );

    /* Configure GPIO to blink user LED D1 at 2Hz rate */
    /* Enable the GPIO port used for user LED 1 */
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);

    /* Enable the GPIO pins for user LED 1 */
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1);
    MAP_GPIOPadConfigSet(GPIO_PORTN_BASE, GPIO_PIN_1,
                         GPIO_STRENGTH_12MA, GPIO_PIN_TYPE_STD);

    xTimer1Start = xTimerStart(xTimer1, 0);

    while(1)
    {

    }
}

void usrLED2Task(void * pvParameters)
{
    /* Create a timer that expires every 2 second */

    xTimer2 = xTimerCreate("Timer2",  /* Name of the timer */
                           200, /* timer period in ticks */
                           pdTRUE,  /* timer will reload itself when it expires */
                           (void *) 2, /* timer ID */
                           timer2CallbackFunc /* timer callback function */
                           );

    /* Configure GPIO to blink user LED D1 at 4Hz rate */
    /* Enable the GPIO port used for user LED 2 */
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);

    /* Enable the GPIO pins for user LED 2 */
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_2);
    MAP_GPIOPadConfigSet(GPIO_PORTN_BASE, GPIO_PIN_2,
                             GPIO_STRENGTH_12MA, GPIO_PIN_TYPE_STD);

    xTimer2Start = xTimerStart(xTimer2, 0);

    while(1)
    {

    }
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
                                                  120000000 /* requested processor frequency */);

    /* ---------------------------------------------------------------------------------------------------------- */
    //TaskHandle_t thdl_usr_led1, thdl_usr_led2;

    /* Create the two tasks that will blink the LEDs at different rates */
    xTaskCreate(usrLED1Task, /* Task function */
                (const portCHAR *)"usrLED1Task", /* Task Name */
                configMINIMAL_STACK_SIZE, /* Task stack size */
                NULL, /* Parameters */
                1, /* Priority */
                NULL /* task handle */ );

    /* Create the two tasks that will blink the LEDs at different rates */
    xTaskCreate(usrLED2Task, /* Task function */
                (const portCHAR *)"usrLED2Task", /* Task Name */
                configMINIMAL_STACK_SIZE, /* Task stack size */
                NULL, /* Parameters */
                1, /* Priority */
                NULL /* task handle */ );

    vTaskStartScheduler();

    return 0;
}
