#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"

#include "driverlib/pin_map.h"
#include "driverlib/sysctl.c"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"

#include "main.h"

int main() {

    /* Here in the main function, we will use the driver library to do
     * the following operations
     *
     * 1. Configure system clock to be at the max (120 MHz)
     * 2. Configure UART to print a string to the terminal saying:
     *      "Project for <Name> <Date>"
     * 3. Configure GPIO to blink user LED D1 at 2Hz rate
     * 4. During each LED blink, print a count that increments for every
     *    blink using UART
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

    /* ---------------------------------------------------------------------------------------------------------- */
    /* Configure UART to print a string to the terminal saying:
     *          "Project for <Name> <Date>" */

    uint8_t ui8_str_to_print[] = "\rProject for Pavan Dhareshwar (04/08/2018)\r\n";

    /* Initialize the UART */
    UART_Init();

    /* Send the string to the terminal */
    UARTSend((uint8_t *)ui8_str_to_print, strlen(ui8_str_to_print));

    /* Wait to receive data on UART */
    while (!gb_uart_data_rcvd)
    {
        /* Delay of 10 ms */
        SysCtlDelay(gui32_sys_clock_rate / (10 * 3));
    }

    /* ---------------------------------------------------------------------------------------------------------- */

    /* ---------------------------------------------------------------------------------------------------------- */
    /* Configure GPIO to blink user LED D1 at 2Hz rate */
    /* During each LED blink, print a count that increments for every
     * blink using UART */

    /* Enable the GPIO port used for user LED 1 */
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);

    /* Enable the GPIO pins for user LED 1 */
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1);

    BlinkUserLED();

    for (;;);

    /* ---------------------------------------------------------------------------------------------------------- */

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
}

void UARTSend(uint8_t *pui8_str, uint32_t ui32_str_len)
{
    gb_uart_data_rcvd = false;

    while (ui32_str_len != 0)
    {
        /* Write a single character to UART */
        UARTCharPut(UART0_BASE, *pui8_str);
        ui32_str_len--;
        pui8_str++;
    }

    gb_uart_data_rcvd = true;
}

void BlinkUserLED(void)
{
    //uint32_t ui32_count = 100;
    static uint32_t ui32_blink_count = 0;
    uint8_t ui8_blink_count_msg[64];

    //while (ui32_count--)
    while(1)
    {
        /* Turn on LED */
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, GPIO_PIN_1);

        /* Delay of 500 ms (2Hz rate) */
        SysCtlDelay(gui32_sys_clock_rate / (2 * 3));

        /* Turn on LED */
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, 0);

        /* Keep track of the number of times the LED blink occurred */
        ui32_blink_count++;

        memset(ui8_blink_count_msg, '\0', sizeof(ui8_blink_count_msg));
        sprintf((char *)ui8_blink_count_msg, "LED Blink Count: %d\r\n", ui32_blink_count);
        UARTSend((uint8_t *)ui8_blink_count_msg, strlen(ui8_blink_count_msg));
    }
}
