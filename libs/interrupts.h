/***************************************************************************//**
 * @file    interrupts.h
 * @author  <your name>
 * @date    <date of creation>
 *
 * @brief   <brief description>
 *
 * Here goes a detailed description if required.
 ******************************************************************************/

#ifndef LIBS_INTERRUPTS_H_
#define LIBS_INTERRUPTS_H_

/******************************************************************************
 * INCLUDES
 *****************************************************************************/

#include <msp430g2553.h>

/******************************************************************************
 * CONSTANTS
 *****************************************************************************/



/******************************************************************************
 * VARIABLES
 *****************************************************************************/



/******************************************************************************
 * FUNCTION PROTOTYPES
 *****************************************************************************/

void interrupts_mode_i2c(void (*receive)(void), void (*transmit)(void));
void interrupts_mode_uart(void (*receive)(void), void (*transmit)(void));


#endif
