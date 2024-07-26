/***************************************************************************//**
 * @file    interrupts.c
 * @author  Amrutha Venkatesan
 * @date    30th June 2023
 *
 * @brief   Common Interrupts for all the USCI interfaces
 *
 * Here goes a detailed description if required.
 ******************************************************************************/

/******************************************************************************
 * INCLUDES
 *****************************************************************************/

#include "./interrupts.h"
#include "./uart.h"

/******************************************************************************
 * VARIABLES
 *****************************************************************************/

void (*forRX)(void);
//void (*forRX_uart)(void);
//void (*forRX_i2c)(void);
void (*forTX)(void);

/******************************************************************************
 * FUNCTION IMPLEMEMTATION
 *****************************************************************************/

void interrupts_mode_i2c(void (*receive)(void), void (*transmit)(void))
{
    forRX = receive;
//    forRX_i2c = receive;
    forTX = transmit;
//    uart_flag = 0;
}

void interrupts_mode_uart(void (*receive)(void), void (*transmit)(void))
{
    forRX = receive;
//    forRX_uart = receive;
    forTX = transmit;
}

#pragma vector = USCIAB0RX_VECTOR
__interrupt void USCIAB0RX_ISR(void)
{
//    if (uart_flag == 1)
//        forRX_uart();
//    else
//        forRX_i2c();

    forRX();
}

#pragma vector = USCIAB0TX_VECTOR
__interrupt void USCIAB0TX_ISR(void)
{
    forTX();
}
