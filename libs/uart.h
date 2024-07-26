/***************************************************************************//**
 * @file    uart.h
 * @author  Amrutha Venkatesan
 * @date    1st June 2023
 * matriculation number - 5169728
 * email - amsvenkat20@gmail.com
 *
 * @brief   UART header file
 *
 * Here goes a detailed description if required.
 ******************************************************************************/

#ifndef LIBS_UART_H_
#define LIBS_UART_H_

/******************************************************************************
 * INCLUDES
 *****************************************************************************/

#include "msp430g2553.h"

/******************************************************************************
 * CONSTANTS
 *****************************************************************************/

#define BUFFER_SIZE 32  // receive buffer array size


/******************************************************************************
 * VARIABLES
 *****************************************************************************/
//int uart_flag = 0;
//// Buffer type definition:
//typedef struct {
//    char data[BUFFER_SIZE];
//    char start;
//    char end;
//    char error;
//} Buffer_t;
//
//// Ring buffer definition:
//Buffer_t ringBuffer = {
//    .start = 0,
//    .end = 0,
//    .error = 0,
//};
//
//// Echo flag definition:
//char echoBack = 0;

/******************************************************************************
 * FUNCTION PROTOTYPES
 *****************************************************************************/
/**
 *
 * Initialization of UART
 *
 */

void uart_disable(void);
void uart_init( void);


/**
 * serialEchoBack
 * This determines if the user's input should be echoed back or not.
 *
 * @param e   0 if no echo is required, anything else if it is.
 */
void serialEchoBack(char e);

/**
 * This function can be used to check for an buffer-error such as a buffer
 * overflow. Calling this function will also reset the error-variable.
 *
 * @return 0 if there is no error, anything elese if there is one.
 */
char serialError(void);

/**
 * Echo one character to the serial connection. Please note that this
 * function will not work with UTF-8-characters so you should stick
 * to ANSI or ASCII.
 *
 * @param char The character to be displayed.
 */
void serialWrite(char tx);

/**
 * Print a given integer as a readable number to serial connection (using
 * the ASCII charmap).
 *
 * @param i   The number to be displayed; 16 bit max.
 */
void serialPrintInt(int i);

/**
 * Print a sequence of characters to the serial connection.
 *
 * @example     serialPrint("output");
 * @param tx    A pointer to the text that shall be printed. Has to be
 *              terminated by \0
 */
void serialPrint(char* tx);

/**
 * Print a sequence of characters to the serial connection and terminate
 * the string with a linebreak. (Note that you'll have to enable "Newline
 * at LF+CR" within HTerm - if you use HTerm.)
 *
 * @example     serialPrint("output");
 * @param tx    A pointer to the text that shall be printed. Has to be
 *              terminated by \0
 */
void serialPrintln(char* tx);

/**
 * Returns 1 if the serial buffer is not empty i.e. some data has been
 * received on the serial connection (e.g. by sending something with HTerm)
 *
 * @return 1 if there is data, 0 if not.
 */
char serialAvailable(void);

/**
 * Clear the serial buffer; all content will be lost.
 */
void serialFlush(void);

/**
 * Returns the first byte from the serial buffer without modifying the
 * same. Returns -1 if the buffer is empty.
 *
 * @return The first byte within the buffer or -1 if the buffer is empty.
 */
int serialPeek(void);

/**
 * Returns the first byte from the serial buffer and removes it from the
 * same. Returns -1 if the buffer is empty.
 *
 * @return The first byte within the buffer or -1 if the buffer is empty.
 */
int serialRead(void);

/**
 * Reads in a number from the serial interface, terminated by any
 * non-numeric character.
 *
 * WARNING: This is a *very basic* implementation and you might want to
 * write your own depending on your scenario and your needs.
 *
 * @return The read-in-number.
 */
int serialReadInt(void);


#endif /* LIBS_UART_H_ */

