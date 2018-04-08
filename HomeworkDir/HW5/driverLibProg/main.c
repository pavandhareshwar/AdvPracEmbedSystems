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

uint32_t gui32_sys_clock_rate;
bool gb_uart_data_rcvd;

void UARTSend(uint8_t *pui8_str, uint32_t ui32_str_len);

void UARTIntHandler(void);

void BlinkUserLED1(void);

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

    //uint8_t ui8_str_to_print[] = "Project for Pavan Dhareshwar (04/04/2018)";

    /* Enable the peripherals */
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    /* Enable processor interrupts */
    ROM_IntMasterEnable();

    /* Configure GPIO A0 and A1 as UART pins */
    GPIOPinConfigure(GPIO_PA0_U0RX); /* UART module 0 receive */
    GPIOPinConfigure(GPIO_PA1_U0TX); /* UART module 0 transmit */

    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    /* Set the configuration of UART 0 */
    ROM_UARTConfigSetExpClk(UART0_BASE, gui32_sys_clock_rate, 115200,
                                (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                                 UART_CONFIG_PAR_NONE));

    /* Enable the UART interrupts */
    ROM_IntEnable(INT_UART0);
    ROM_UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);

    /* Enable loopback mode for UART0 */
    UARTLoopbackEnable(UART0_BASE);

    /* Send the string to the terminal */
    //UARTSend((uint8_t *)ui8_str_to_print, strlen(ui8_str_to_print));

    UARTSend((uint8_t *)"ABCDE", 6);

    /* Wait to receive data on UART */
    while (!gb_uart_data_rcvd)
    {
        /* Delay of 10 ms */
        SysCtlDelay(gui32_sys_clock_rate / (100 * 3));
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

    BlinkUserLED1();

    /* ---------------------------------------------------------------------------------------------------------- */

    return 0;
}

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
void UARTSend(uint8_t *pui8_str, uint32_t ui32_str_len)
{
    gb_uart_data_rcvd = false;

    while (ui32_str_len != 0)
    {
        /* Write a single character to UART */
        ROM_UARTCharPutNonBlocking(UART0_BASE, *pui8_str);
        ui32_str_len--;
        pui8_str++;
    }
}

/**
 *  @brief Interrupt handler for UART
 *
 *  This function will serve as the interrupt handler for UART
 *
 *  @param  void
 *
 *  @return void
*/
void UARTIntHandler(void)
{
    uint32_t ui32_int_status;

    /* Get the interrupt status */
    ui32_int_status = ROM_UARTIntStatus(UART0_BASE, true);

    /* Clear the interrupt */
    ROM_UARTIntClear(UART0_BASE, ui32_int_status);

    /* Determine if there are any characters in the receive FIFO and read them */
    while (ROM_UARTCharsAvail(UART0_BASE))
    {
        UARTCharPut(UART0_BASE,
                    ROM_UARTCharGetNonBlocking(UART0_BASE));
    }

    gb_uart_data_rcvd = true;
}

/**
 *  @brief User LED blink function
 *
 *  This function will blink user LED 1
 *
 *  @param  void
 *
 *  @return void
*/
void BlinkUserLED1(void)
{
    uint32_t ui32_count = 10;
    static uint32_t ui32_blink_count = 0;

    while (ui32_count--)
    {
        /* Turn on LED */
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, GPIO_PIN_1);

        /* Delay of 1 ms */
        /* TODO: Change this to 2 Hz delay */
        SysCtlDelay(gui32_sys_clock_rate / (1000 * 3));

        /* Turn on LED */
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, 0);

        /* Keep track of the number of times the LED blink occurred */
        ui32_blink_count++;

        ROM_UARTCharPutNonBlocking(UART0_BASE, (char)ui32_blink_count);
    }
}
