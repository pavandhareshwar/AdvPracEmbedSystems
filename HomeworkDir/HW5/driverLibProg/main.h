/*
 * main.h
 *
 *  Created on: Apr 6, 2018
 *      Author: pavandhareshwar
 */

#ifndef MAIN_H_
#define MAIN_H_

uint32_t gui32_sys_clock_rate;
bool gb_uart_data_rcvd;

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

/**
 *  @brief Interrupt handler for UART
 *
 *  This function will serve as the interrupt handler for UART
 *
 *  @param  void
 *
 *  @return void
*/
void UARTIntHandler(void);

/**
 *  @brief User LED blink function
 *
 *  This function will blink user LED
 *
 *  @param  void
 *
 *  @return void
*/
void BlinkUserLED(void);

#endif /* MAIN_H_ */
