/***************************************************************************//**
 * @file    adac.h
 * @author  Amrutha Venkatesan
 * @date    1st June 2023
 * matriculation number - 5169728
 * email - amsvenkat20@gmail.com
 *
 * @brief   header file for ADAC
 *
 * Here goes a detailed description if required.
 ******************************************************************************/

#ifndef EXERCISE_LIBS_ADAC_H_
#define EXERCISE_LIBS_ADAC_H_

/******************************************************************************
 * INCLUDES
 *****************************************************************************/

#include "./i2c.h"

/******************************************************************************
 * CONSTANTS
 *****************************************************************************/



/******************************************************************************
 * VARIABLES
 *****************************************************************************/

int init ;

/******************************************************************************
 * FUNCTION PROTOTYPES
 *****************************************************************************/

// All functions return 0 if everything went fine
// and anything but 0 if not.

// Initialize the ADC / DAC
unsigned char adac_init(void);

// Read all ADC-values and write it into the passed values-array.
// (Important: always pass an array of size four (at least).) (1 pt.)
unsigned char adac_read(unsigned char * values);

// Write a certain value to the DAC. (1 pt.)
unsigned char adac_write(unsigned char value);

#endif /* EXERCISE_LIBS_ADAC_H_ */
