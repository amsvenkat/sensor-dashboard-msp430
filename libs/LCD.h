/***************************************************************************//**
 * @file    LCD.h
 * @author  Amrutha Venkatesan
 * @date    13th May 2023
 *
 * @brief   Exercise 1 - Display Interface
 *
 * Functions declarations needed for the LCD interface
 ******************************************************************************/

#ifndef LIBS_LCD_H_
#define LIBS_LCD_H_

/******************************************************************************
 * INCLUDES
 *****************************************************************************/

#include <msp430g2553.h>
#include <stdio.h>
#include <stdlib.h>

/******************************************************************************
 * CONSTANTS
 *****************************************************************************/



/******************************************************************************
 * VARIABLES
 *****************************************************************************/

unsigned char *ptr;

/******************************************************************************
 * FUNCTION PROTOTYPES
 *****************************************************************************/

void read_cgram(unsigned char location);
void write_cgram(unsigned char *data);


/** Initialization */

// Initialization of the LCD; set all pin directions,
// basic setup of the LCD, etc. (1 pt.)
void lcd_init (void);

/** Helper Functions */
void delay_us(unsigned int us);
void delay_ms(unsigned int ms);

/** Control functions */

// Enable (1) or disable (0) the display (i.e. hide all text) (0.5 pts.)
void lcd_enable (unsigned char on);

// Set the cursor to a certain x/y-position (0.5 pts.)
void lcd_cursorSet (unsigned char x, unsigned char y);

// Show (1) or hide (0) the cursor (0.5 pts.)
void lcd_cursorShow (unsigned char on);

// Blink (1) or don't blink (0) the cursor (0.5 pts.)
void lcd_cursorBlink (unsigned char on);


/** Data manipulation */

// Delete everything on the LCD (1 pt.)
void lcd_clear (void);

// Put a single character on the display at the cursor's current position (1 pt.)
void lcd_putChar (char character);

// Show a given string on the display. If the text is too long to display,
// don't show the rest (i.e. don't break into the next line) (1 pt.).
void lcd_putText (char * text);

// Show a given number at the cursor's current position.
// Note that this is a signed variable! (1 pt.)
void lcd_putNumber (int number);

#endif /* LIBS_LCD_H_ */
