/***************************************************************************//**
 * @file    i2c.c
 * @author  Amrutha Venkatesan
 * @date    30th June 2023
 * @brief   Implementation of I2C using common ISRs
 *
 * Here goes a detailed description if required.
 ******************************************************************************/

#include "./i2c.h"
#include "./interrupts.h"

/******************************************************************************
 * VARIABLES
 *****************************************************************************/

// 1 if all bytes have been sent, 0 if transmission is still ongoing.
unsigned char transferFinished = 0;
unsigned char nack = 0;
int check;
int counter;  // Counter flag for the increment purpose in the array used in ISR

/******************************************************************************
 * LOCAL FUNCTION PROTOTYPES
 *****************************************************************************/

void free_bus(void);
void i2c_RXISR(void);
void i2c_TXISR(void);

/******************************************************************************
 * LOCAL FUNCTION IMPLEMENTATION
 *****************************************************************************/

void free_bus()
{

    P1DIR |= (BIT6 | BIT7);
    int i = 0;
    for (i = 0; i < 8; i++)
    {
        P1OUT |= BIT6;
        _delay_cycles(16 * 5);
        P1OUT &= ~BIT6;
        _delay_cycles(16 * 5);

    }
}

void i2c_RXISR(void)
{
    if (UCB0STAT & UCNACKIFG)
    {
        nack = 1;
        UCB0STAT &= ~UCNACKIFG;
        transferFinished = 1;
    }
}

void i2c_TXISR(void)
{
    if (IFG2 & UCB0TXIFG)
    {
        if (TxByteCtr == 0)                            // Check TX byte length
        {
            //UCB0CTL1 |= UCTXSTP;
            IFG2 &= ~UCB0TXIFG;
            transferFinished = 1;
        }
        else
        {
            UCB0TXBUF = PTxData[counter];              // Load TX buffer
            TxByteCtr--;
            counter++;
        }

    }

    if (IFG2 & UCB0RXIFG)                              // Receive Interrupt flag
    {
        if (counter < RxByteCtr)
        {
            int rx_val = UCB0RXBUF;                     // Copy from RX buffer
            PRxData[counter] = rx_val;
            counter++;

            if (counter == (RxByteCtr - 1)) // Send STOP signal before the last byte is received to make the slave not send further bytes
                UCB0CTL1 |= UCTXSTP;

        }
        if (counter == RxByteCtr)

        {
            IFG2 &= ~UCB0RXIFG;
            transferFinished = 1;
        }
    }

}

/******************************************************************************
 * FUNCTION IMPLEMENTATION
 *****************************************************************************/

// TODO: Implement these functions:
void i2c_init(unsigned char addr)
{

    P1DIR |= (BIT6 | BIT7);
    P1OUT &= ~(BIT6 | BIT7);

    free_bus();                                // Free bus at the start

    // Route to I2C bus
//    P1DIR |= BIT3;                             // Make output pin
//    P1OUT |= BIT3;                             // Enable I2C line
    P3DIR |= BIT3;                             // Make output pin
    P3OUT |= BIT3;                             // Enable I2C line

    interrupts_mode_i2c(i2c_RXISR, i2c_TXISR);   // For choosing i2c ISRs

    UCB0CTL1 |= UCSWRST;                       // Enable SW reset

    //UCBR = f_SMCLK / f_BitClock = 1 MHz / 100 kHz = 10
    UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;     // I2C Master, synchronous mode
    UCB0CTL1 = UCSSEL_2 + UCSWRST;            // Use SMCLK, keep SW reset
//    UCB0BR0 = 160;                            // fSCL = SMCLK/12 = ~100kHz
//    UCB0BR1 = 0;
    UCB0BR0 = 10;                             // fSCL = SMCLK/12 = ~100kHz
    UCB0BR1 = 0;

    P1SEL |= (BIT6 | BIT7);                   // Assign I2C pins to USCI_B0
    P1SEL2 |= (BIT6 | BIT7);

    UCB0I2CSA = addr;                   // Set slave address - 0100 1000 for ADC
    IE2 |= UCB0TXIE | UCB0RXIE;

    UCB0CTL1 &= ~UCSWRST;                    // Clear SW reset, resume operation

    IFG2 &= ~(UCB0TXIFG | UCB0RXIFG);         // Clear Interrupt flags
    UCB0STAT &= ~UCNACKIFG;

    UCB0I2CIE |= UCNACKIE;                    // Enable NACK interrupt

}

unsigned char i2c_write(unsigned char length, unsigned char *txData,
                        unsigned char stop)
{
    // Before writing, you should always check if the last STOP-condition has already been sent.
    while (UCB0CTL1 & UCTXSTP)
        ;

    unsigned char ret = 1;
    transferFinished = 0;
    TxByteCtr = length;
    PTxData = txData;
    counter = 0;

    IE2 |= UCB0TXIE;
    //IE2 &= ~UCB0RXIE;

    UCB0CTL1 |= UCTR + UCTXSTT;               // I2C TX, start condition

    while (!transferFinished)
        ;

    if (stop != 0)     // if stop =1  condition given,  the STOP is transmittted
    {
        UCB0CTL1 |= UCTXSTP;
    }

    if (!nack)  // if transmitted byte is not acknowledged by the slave return 1
        ret = 0;
    nack = 0;

    return ret;
}

void i2c_read(unsigned char length, unsigned char *rxData)
{
    // Before writing, you should always check if the last STOP-condition has already been sent.
    while (UCB0CTL1 & UCTXSTP)
        ;
    transferFinished = 0;
    RxByteCtr = length;

    PRxData = rxData;
    counter = 0;

    IE2 |= UCB0RXIE;
    IE2 &= ~UCB0TXIE;

    if (length == 1)
    {
        UCB0CTL1 &= ~UCTR;                   // Clearing for Receiving
        UCB0CTL1 |= UCTXSTT;                 // I2C start condition

        while (UCB0CTL1 & UCTXSTT)
            ;
        UCB0CTL1 |= UCTXSTP;                 // I2C RX stop condition
    }
    else
    {
        UCB0CTL1 &= ~UCTR;
        UCB0CTL1 |= UCTXSTT;
        //__delay_cycles(160000);
    }
    while (!transferFinished)
        // Waiting for the transfer to be finished
        ;

}

//#pragma vector = USCIAB0TX_VECTOR
//__interrupt void USCIAB0TX_ISR(void)
//{
//
//    if (IFG2 & UCB0TXIFG)                              // UDAC connected to BCKL
//    {
//        if (TxByteCtr == 0)                            // Check TX byte length
//        {
//            //UCB0CTL1 |= UCTXSTP;
//            IFG2 &= ~UCB0TXIFG;
//            transferFinished = 1;
//        }
//        else
//        {
//            UCB0TXBUF = PTxData[counter];              // Load TX buffer
//            TxByteCtr--;
//            counter++;
//        }
//
//    }
//
//    if (IFG2 & UCB0RXIFG)                               // Receive Interrupt flag
//    {
//        if (counter < RxByteCtr)
//        {
//            int rx_val = UCB0RXBUF;                     // Copy from RX buffer
//            PRxData[counter] = rx_val;
//            counter++;
//
//            if (counter == (RxByteCtr - 1))             // Send STOP signal before the last byte is received to make the slave not send further bytes
//                UCB0CTL1 |= UCTXSTP;
//
//        }
//        if (counter == RxByteCtr)
//
//        {
//            //IFG2 &= ~UCB0RXIFG;
//            transferFinished = 1;
//        }
//    }
//
//}
//
//#pragma vector = USCIAB0RX_VECTOR
//__interrupt
//void USCIAB0RX_ISR(void)
//{
//    if (UCB0STAT & UCNACKIFG)
//    {
//        nack = 1;
//        UCB0STAT &= ~UCNACKIFG;
//        transferFinished = 1;
//    }
//}
