/***************************************************************************//**
 * @file    LCD.c
 * @author  Amrutha Venkatesan
 * @date    13th May 2023
 *
 * @brief   Exercise 1 - Display Interface
 *
 * Macros have been created for RW, RS, E_HIGH and E_LOW for convenience
 * All the LCD library functions have been implemented in this section
 * Helper functions implemented :
 * 1. enable() - to generate pulse
 * 2. send_data() - for writing into the LCD - DR
 * 3. send_command() - for sending instructions to the LCD - IR
 * 4. send_nibble() - used in the initialization() to help with the function setting
 * 5. delay_ms() ,delay_us() - for adding delays throughout when needed
 ******************************************************************************/

#include "./LCD.h"


/******************************************************************************
 * VARIABLES
 *****************************************************************************/
#define RS BIT0
#define RW BIT1
#define E_HIGH P3OUT = P3OUT | BIT2
#define E_LOW P3OUT = P3OUT & (~BIT2)
#define D4 BIT0
#define D5 BIT1
#define D6 BIT2
#define D7 BIT3

/******************************************************************************
 * LOCAL FUNCTION PROTOTYPES
 *****************************************************************************/

/******************************************************************************
 * LOCAL FUNCTION IMPLEMENTATION
 *****************************************************************************/
void delay_us(unsigned int us)
{
  while (us)
  {
    // 1 for 1 MHz
    // 16 cycles done in 1 us - 16MHz
    __delay_cycles(1);
    us--;
  }
}

void delay_ms(unsigned int ms)
{
  while (ms)
  {
     // 1000 for 1MHz
     // 16000 cycles done in 1ms
    __delay_cycles(1000);
    ms--;
  }
}

// Generate the Pulse
void enable(void)
{
    E_HIGH;
    delay_ms(10);
    E_LOW;
}

void send_data(unsigned char data)
{
    unsigned char lower_nibble = 0x0F & (data);
    unsigned char higher_nibble = 0x0F & (data >> 4);
    delay_us(100);

    // send higher nibble
    P2OUT =  (higher_nibble);
    P3OUT &= ~(RW);
    P3OUT |= RS;
    enable();

    delay_ms(10);
    // send lower nibble
    P2OUT =  (lower_nibble);
    P3OUT &= ~(RW);
    P3OUT |= RS;
    enable();
    delay_ms(10);
}

void send_command(unsigned char cmd)
{
    unsigned char lower_nibble = 0x0F & (cmd);
    unsigned char higher_nibble = 0x0F & (cmd >> 4);
    delay_us(100);

    // higher nibble
    P2OUT = (higher_nibble);
    P3OUT &= ~(RW | RS);
    enable();
    delay_ms(10);

    // lower nibble
    P2OUT = (lower_nibble);
    P3OUT &= ~(RW | RS);
    enable();
    delay_ms(10);
}


void send_nibble(unsigned char cmd)
{
    P2OUT = cmd;
    P3OUT &= ~( RW | RS);
    enable();
    delay_ms(10);
}


void create_cgram(unsigned char *data, unsigned char location)
{
    // Set the address

    send_command(0x40+(location*8));
    delay_us(50);

    // Write into the CGRAM
    int i;
    for (i=0; i<8; i++)
    {
        send_data(data[i]);
        delay_us(50);
    }

    send_command(0x2);
    delay_us(50);

    delay_ms(1000);

    send_data(0x00);

//    delay_us(50);
//    send_command(0x00);


}

void read_cgram(unsigned char location)
{
    P3OUT |= (RW | RS);
    unsigned char addr = 0x40+(location*8);
    send_command(addr);
    delay_us(50);
}

/******************************************************************************
 * FUNCTION IMPLEMENTATION
 *****************************************************************************/

void lcd_init(void)
{
    P3DIR |= 0xFF;
    P2DIR |= 0xFF;
    P2OUT &= 0x00;

    delay_ms(50); // Power ON and wait for more than 40ms
    send_nibble(0x03);

    delay_ms(10); // Wait 4.1 ms
    send_nibble(0x03);

    delay_us(150);  // Wait 100 us
    send_nibble(0x03);

    delay_us(200);
    send_nibble(0x02);  // 4 bit mode 0010

    delay_ms(200);
    send_nibble(0x2);
    delay_ms(10);
    send_nibble(0x8);  // 4 bit mode with 2 lines 0010 1000

    delay_ms(20);
    send_nibble(0x0);
    delay_ms(20);
    send_nibble(0xE); // display off 0000 1000

    delay_ms(20);
    send_nibble(0x0);
    delay_ms(20);
    send_nibble(0x1); // display clear 0000 0001

    delay_ms(20);
    send_nibble(0x0);
    delay_ms(20);
    send_nibble(0x06); // increment cursor 0000 0110

    delay_ms(20);
    send_nibble(0x0);
    delay_ms(20);
    send_nibble(0x08); // return home

}

void lcd_enable(unsigned char on)
{
    /**
     * Input : 1 or 0
     * Enable the LCD display
     */
    if (on == 1)
    {
        delay_ms(20);
        send_nibble(0x0);
        delay_ms(20);
        send_nibble(0xC);  //  0000 1100
    }
    else
        delay_ms(20);
        send_nibble(0x0);
        delay_ms(20);
        send_nibble(0x8); //  0000 1000
}

void lcd_cursorSet(unsigned char x, unsigned char y)
{
    /**
     * Input : x  = 1 to 2 , y = 1 to 16
     * sets the cursor to the specified x and y position
     */
    y--;

    switch (x)
    {
    case 1:
        send_command((y |= 0x80)); // 1000 0000
        break;
    case 2:
        send_command((y |= 0xC0)); // 1100 0000
    default:
        break;
    }
}

void lcd_cursorShow(unsigned char on)
{
    /**
     * Input : 1 or 0
     * Show the cursor
     */
    if (on == 1)
    {

        delay_ms(20);
        send_nibble(0x0);
        delay_ms(20);
        send_nibble(0xE); ; //  0000 1110
    }
    else
    {

        delay_ms(20);
        send_nibble(0x0);
        delay_ms(20);
        send_nibble(0xC);  //  0000 1100
    }

}

void lcd_cursorBlink(unsigned char on)
{
    /**
     * Input : 1 or 0
     * Make the cursor to blink
     */
    if (on == 1)
    {
        delay_ms(20);
        send_nibble(0x0);
        delay_ms(20);
        send_nibble(0xF);  //  0000 1111
    }
    else
    {

        delay_ms(20);
        send_nibble(0x0);
        delay_ms(20);
        send_nibble(0xE);  //  0000 1110
    }
}

void lcd_clear(void)
{
    /**
     * Clear the display
     */
    delay_ms(20);
    send_nibble(0x0);
    delay_ms(20);
    send_nibble(0x1);  // 0000 0001
    delay_ms(20);
}

void lcd_putChar(char character)

{
    /**
     * Input : single character
     * Character displayed at the required position
     */
    send_data(character);
}

void lcd_putText(char *text)
{
    /**
     * Input : Text
     * Text displayed at the required position
     */
    int i;
    for(i=0;text[i]!=0;i++)
        send_data(text[i]);
}

void lcd_putNumber(int number)
{
    /**
     * Input : signed number
     * Number displayed at the required position
     * Convert number to string and pass to LCD
     */

    char c[5];
    sprintf(c, "%d", number);
    int j;
    for (j=0 ;c[j]!=0;j++)
        send_data(c[j]);
    //lcd_putChar(c);

}
