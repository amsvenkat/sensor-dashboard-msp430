/***************************************************************************//**
 * @file    adac.c
 * @author  Amrutha Venkatesan
 * @date    30th June 2023
 *
 * @brief   ADAC source file
 *
 * Here goes a detailed description if required.
 ******************************************************************************/

#include "./adac.h"

/******************************************************************************
 * VARIABLES
 *****************************************************************************/

/******************************************************************************
 * LOCAL FUNCTION PROTOTYPES
 *****************************************************************************/

/******************************************************************************
 * LOCAL FUNCTION IMPLEMENTATION
 *****************************************************************************/

/******************************************************************************
 * FUNCTION IMPLEMENTATION
 *****************************************************************************/

unsigned char adac_init(void)
{
//    unsigned char addr = 0x48;
//    i2c_init(addr);
//    return 0;

    unsigned char addr = 0x48;
    i2c_init(addr);
    unsigned char t[1] = { 0x44 };
    return i2c_write(1, t, 1);
}

unsigned char adac_read(unsigned char *values)
{
    i2c_read(5, values);
    return 0;
}

unsigned char adac_write(unsigned char value)
{
    unsigned char r[2] = { 0x44, value };
//    if (init == 1)
//    {
//        init = 0;
//        return i2c_write(1, r, 1);
//
//    }
//    else
    i2c_write(2, r, 1);

    return 0;
}
