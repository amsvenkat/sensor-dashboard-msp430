/***************************************************************************//**
 * @file    sensor.c
 * @author  Amrutha Venkatesan
 * @date    28th July 2023
 *
 * @brief   Sensor Readings
 *
 * U_NTC   P1.0 (shared)
 * LDR     P1.3
 * U_POT   P1.4
 *
 * LED gn          P3.0 (shared)
 * LED rt          P3.1 (shared)
 * X11 -PB5        P3.5
 * X12 -PB6        P3.6
 * X5 -REL_STAT    P3.4
 * X10             REL_ID
 *
 *
 * Here goes a detailed description if required.
 */
/*****************************************************************************/

#include "./sensor.h"

/******************************************************************************
 * VARIABLES
 *****************************************************************************/

/******************************************************************************
 * FUNCTION IMPLEMENTATION
 *****************************************************************************/

void clockReset(void)
{
    P2OUT |= BIT4;
    P2OUT &= ~BIT4;
}

void sensor_init()
{
    //Turn ADC on;
    ADC10CTL0 = ADC10ON + ADC10SHT_2;

    // Setting directions for shift registers
    P2DIR |= (BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT6);
    P2DIR &= ~BIT7;

    // XTAL pins made to I/O
    P2SEL &= ~(BIT6 | BIT7);

    // Resetting Shift registers
    P2OUT &= ~BIT5;
    P2OUT |= BIT5;

    // CLK
    P2OUT &= ~BIT4;

    // Set pin direction for NTC, LDR, U_POT
    P1DIR &= ~(BIT0 | BIT3 | BIT4);

    // Set pin direction for pushbuttons
    P3DIR &= ~(BIT5 | BIT6);
    P3REN |= (BIT5 | BIT6);
    P3OUT |= (BIT5 | BIT6);

}

int get_ntc()
{
    ADC10AE0 |= BIT0;
    ADC10CTL1 = INCH_0;

    ADC10CTL1 &= ~(INCH_3 | INCH_4);

    ADC10CTL0 |= ENC + ADC10SC;

    while (ADC10CTL1 & ADC10BUSY)
        ;

    int ntc_value = ADC10MEM;

    ADC10CTL0 &= ~(ENC);

    ADC10AE0 &= ~BIT0;

    return ntc_value;

}

int get_ldr()
{
    ADC10AE0 |= BIT3;
    ADC10CTL1 = INCH_3;

    ADC10CTL1 &= ~(INCH_0 | INCH_4);

    ADC10CTL0 |= ENC + ADC10SC;

    while (ADC10CTL1 & ADC10BUSY)
        ;

    int ldr_value = ADC10MEM;

    ADC10CTL0 &= ~(ENC);

    ADC10AE0 &= ~BIT3;
    return ldr_value;
}

int get_pot()
{

    ADC10AE0 |= BIT4;
    ADC10CTL1 = INCH_4;

    ADC10CTL1 &= ~(INCH_0 | INCH_3);

    ADC10CTL0 |= ENC + ADC10SC;

    while (ADC10CTL1 & ADC10BUSY)
        ;

    int pot_value = ADC10MEM;

    ADC10CTL0 &= ~(ENC);
    ADC10AE0 &= ~BIT4;

    return pot_value;
}

int get_pb()
{
    int pb_value = 0;
    int i;

    // Shift register 1 mode parallel mode (S0 = 1, S1 = 1)
    P2OUT |= (BIT2 | BIT3);

    // Shift register 2 mode stop mode (S0 = 0, S1 = 0)
    P2OUT &= ~(BIT0 | BIT1);

    clockReset();

    // For PB 6
    if (!(P3IN & BIT6))
        pb_value = (pb_value << 1) + 1;
    else
        pb_value = (pb_value << 1);

    // For PB 5
    if (!(P3IN & BIT5))
        pb_value = (pb_value << 1) + 1;
    else
        pb_value = (pb_value << 1);

    // For PB4 to PB1
    // Shift register 1 mode right shift mode (S0 = 1, S1 = 0)*/
    P2OUT |= BIT2;
    P2OUT &= ~BIT3;

    for (i = 0; i <= 2; i++)
    {
        if (P2IN & BIT7)
            pb_value = (pb_value << 1) + 1;
        else
            pb_value = (pb_value << 1);
        clockReset();
    }

    if (P2IN & BIT7)
        pb_value = (pb_value << 1) + 1;
    else
        pb_value = (pb_value << 1);

    return pb_value;

}

