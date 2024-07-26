/***************************************************************************//**
 * @file    main.c
 * @author  Amrutha Venkatesan
 * @date    27th July 2023
 *
 *
 * @brief   Exercise 5 - Project
 *
 * Implementation of SensorDashBoard - Display of Sensor Data and Controlling a few others
 *
 * PIN Connections
 *
 * Ultrasonic sensor Connections
 * US-CLK   P3.3 (shared) with I2C_SPI pin
 * Rx-COMP  P1.0 (shared) with U_NTC
 *
 * Sensors - U_POT, NTC, LDR
 * U_NTC   P1.0 (shared) with RX_COMP
 * LDR     P1.3
 * U_POT   P1.4
 * PB5     P3.5
 * PB6     P3.6
 *
 * Shift Registers
 * P2.0		S0(2)
 * P2.1		S1(2)
 * P2.2		S0(1)
 * P2.3		S1(1)
 * P2.4		CK
 * P2.5		/CLR
 * P2.6		SR2
 * P2.7		QD1
 *
 * I2C/SPI PINS
 * I2C_SPI     P3.3 (shared) with US_CLK
 * XSCL        P1.6
 * XSDA        P1.7
 *
 * Actuators
 * LED GREEN    P1.5
 * LED RED      P3.7
 * LED BLueX10  X3 (REL_ID)
 * REL_STAT     P3.4
 *
 * LCD connections
 * RS    P3.0 with LED GR
 * R/W   P3.1 with LED RED
 * E     P3.2
 * D4    P2.0
 * D5    P2.1
 * D6    P2.2
 * D7    P2.3
 * JP5 to left
 *
 * @note    The project was exported using CCS 12.0.0.
 *
 ******************************************************************************/

#include "libs/templateEMP.h"   // UART disabled, see @note!
#include "string.h"
#include "stdio.h"
#include "libs/uart.h"
#include "libs/sensor.h"
#include "libs/adac.h"
#include "libs/i2c.h"
#include "libs/mma.h"
#include "libs/lcd.h"

int distance;
double acc_values[3] = { 0, 0, 0 };
int pot, ldr, ntc, pb;
char adc_values[5] = { 0, 0, 0, 0, 0 };

char input[64];
char cmd_stored[64];
char input_cmd[64];
char strmem_b[64];
char led_array[6] = { 0, 0, 0, 0, 0, 0 };

int flag = 0;
double range = 0.0;
int dboard_flag = 0;
int time_counter = 0;
int disp_flag = 0;
int index = 0;
int process_flag = 0;
char check[3];
int exit_dash = 0;
int init_dash = 0;
int user_mode = 1;
int cmd_wrong = 0;

void relay_control()
{
    if (strcmp(cmd_stored, "relay on\r") == 0)
    {
        P3DIR |= BIT4;
        P3OUT |= BIT4;
    }
    else if (strcmp(cmd_stored, "relay off\r") == 0)
    {
        P3DIR |= BIT4;
        P3OUT &= ~BIT4;
    }
    else
        cmd_wrong = 1;
    //serialPrint("\e[1B\e[0E\e[2KWrong Relay Command");

}

void shift_register_init()
{
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

}

// Setting mode of shift register 2
void shiftModeRight()
{
    P2OUT |= BIT0;
    P2OUT &= ~BIT1;
}
// Setting mode of shift register 2
void shiftModeLeft()
{
    P2OUT |= BIT1;
    P2OUT &= ~BIT0;
}

void led_control()
{
    int i = 0;

    char led[10];
    memset(led, NULL, 10);

    memcpy(led, &cmd_stored[0], 6);

    //serialPrint(led);

    if ((strcmp(led, "led d6") == 0) || (strcmp(led, "led d5") == 0))
    {

        if (strcmp(cmd_stored, "led d5 on\r") == 0)
        {
            led_array[4] = 1;

        }
        if (strcmp(cmd_stored, "led d5 off\r") == 0)
        {
            led_array[4] = 0;
        }
        if (strcmp(cmd_stored, "led d6 on\r") == 0)
        {
            led_array[5] = 1;
        }
        if (strcmp(cmd_stored, "led d6 off\r") == 0)
        {
            led_array[5] = 0;
        }

        // LED green 3.0  1.5
        if (led_array[4] == 1)
        {
            P1DIR |= BIT5;
            P1OUT |= BIT5;

        }
        else
        {
            P1DIR |= BIT5;
            P1OUT &= ~BIT5;

        }

        // LED red 3.1  3.7
        if (led_array[5] == 1)
        {
            P3DIR |= BIT7;
            P3OUT |= BIT7;

        }
        else
        {
            P3DIR |= BIT7;
            P3OUT &= ~BIT7;

        }
    }

    else if ((strcmp(led, "led d1") == 0) || (strcmp(led, "led d2") == 0)
            || (strcmp(led, "led d3") == 0) || (strcmp(led, "led d4") == 0))
    {
        if (strcmp(cmd_stored, "led d1 on\r") == 0)
        {
            led_array[0] = 1;
        }
        if (strcmp(cmd_stored, "led d1 off\r") == 0)
        {
            led_array[0] = 0;
        }
        if (strcmp(cmd_stored, "led d2 on\r") == 0)
        {
            led_array[1] = 1;
        }
        if (strcmp(cmd_stored, "led d2 off\r") == 0)
        {
            led_array[1] = 0;
        }
        if (strcmp(cmd_stored, "led d3 on\r") == 0)
        {
            led_array[2] = 1;
        }
        if (strcmp(cmd_stored, "led d3 off\r") == 0)
        {
            led_array[2] = 0;
        }
        if (strcmp(cmd_stored, "led d4 on\r") == 0)
        {
            led_array[3] = 1;
        }
        if (strcmp(cmd_stored, "led d4 off\r") == 0)
        {
            led_array[3] = 0;
        }

        //shift_register_init();

        shiftModeRight();
        //clockReset();
        for (i = 3; i >= 0; i--)
        {
            if (led_array[i] == 0)
                P2OUT &= ~BIT6;
            else
                P2OUT |= BIT6;

            clockReset();
        }

        // Hold the LED values
        P2OUT &= ~(BIT0 | BIT1);
        //clockReset();
    }
    else
        cmd_wrong = 1;
    //serialPrint("\e[1B\e[0E\e[2KWrong LED Command");

}

void write_to_lcd(char *s)
{
    delay_ms(50);
    lcd_enable(1);

    delay_ms(200);
    lcd_cursorSet(1, 1);

    delay_ms(200);
    lcd_cursorShow(1);

    delay_ms(1000);
    lcd_putText(s);
//
//    delay_ms(1000);
//    lcd_clear();
}

void lcd_control()
{
    lcd_init();
    delay_ms(20);

    char lcd_cmd[10];
    char text[32];
    memset(lcd_cmd, NULL, 10);
    memset(text, NULL, 32);
    int i;

    for (i = 0; i <= 8; i++)
        lcd_cmd[i] = cmd_stored[i];

    if (strcmp(lcd_cmd, "lcd clear") == 0)

    {
        lcd_clear();
        delay_ms(10);
    }

    else if (strcmp(lcd_cmd, "lcd print") == 0)
    {
        // handle if not needed
        memcpy(text, &cmd_stored[10], 16);

        // Removing \r from string
        int b = 0;
        for (b = 0; b < 16; b++)
        {
            if (text[b] == '\r')
                text[b] = NULL;
        }

        //char s[64] = { "Hello!" };
        write_to_lcd(text);
    }
    else
        cmd_wrong = 1;
    //serialPrint("\e[1B\e[0E\e[2KWrong LCD command");

}

void process_command()
{
    int j;

    for (j = 0; j < 3; j++)
        check[j] = cmd_stored[j];

    if (strcmp(check, "led") == 0)
        led_control();

    else if (strcmp(check, "lcd") == 0)
        lcd_control();

    else if (strcmp(check, "rel") == 0)
        relay_control();

    else if (strcmp(check, "exi") == 0)
    {
        exit_dash = 1;
    }
    else
        cmd_wrong = 1;

    if (cmd_wrong == 1)
    {
        serialPrint("Wrong Command. Follow Format");
        cmd_wrong = 0;
    }

    memset(cmd_stored, NULL, 64);
}

void refresh_timer_start()
{

// Set mode for 0.5sec - SMCLK, divider -8, mode - up
    TA1CTL |= TACLR;
    TA1CTL = TASSEL_2 + ID_3;
    TA1CTL |= MC_1;
    TA1CCR0 = 62500;
    TA1CCTL0 |= CCIE;
    TA1CCTL0 &= ~CCIFG;

    time_counter = 0;
}

void refresh_timer_stop()
{
    TA1CTL |= MC_0;  // Stop timer
}

void us_timer_init(void)
{
// CLK - SMCLK at 1MHz
    TA0CTL |= TACLR;
    TA0CTL = TASSEL_2;

// Clear Interrupt flag
    TA0CCTL0 &= ~CCIFG;

// Capture mode , both rising and falling edge , with GND as CCS signal
    TA0CCTL0 |= CAP + CM_3 + CCIS_2;
}

void get_us_dist(void)
{

    unsigned char pulse_count;
// Clock for US device
    P3DIR |= BIT3;
    P3OUT &= ~BIT3;

// Pull up for the R-COMP
    P1REN |= BIT0;
    P1OUT |= BIT0;

// init timer
    us_timer_init();

// Clear interrupt flag, high Low edge and enable
    P1IFG &= ~BIT0;
    P1IES |= BIT0;
    P1IE |= BIT0;

// Stop capture timer
//TA0CTL &= ~MC_2;

// Wait for 1ms
    __delay_cycles(1000);

// Start sending pulses with 40 KHz for the trigger
    for (pulse_count = 0; pulse_count < 8; pulse_count++)
    {
        P3OUT |= BIT3;
        __delay_cycles(7);
        P3OUT &= ~BIT3;
        __delay_cycles(1);
    }

// Reset TAR
    TA0CTL |= TACLR;

// Start capture timer
    TA0CTL |= MC_2;

    __delay_cycles(10000);

}

void get_joystick()
{
    adac_init();
    // Read all 4 channel values from ADAC
    adac_read(adc_values);
}

void get_acceleration()
{
    mma_init();
    acc_values[0] = mma_getRealX();
    acc_values[1] = mma_getRealY();
    acc_values[2] = mma_getRealZ();

}

void disp_init()
{
    serialPrint("\e[2J");
    serialPrint("\e[1;0HSENSOR DASHBOARD");
    serialPrint("\e[3;0HUltrasonic Sensor: ");
    serialPrint("\e[5;0HAcceleration Sensor X: ");
    serialPrint("\e[7;0HAcceleration Sensor Y: ");
    serialPrint("\e[9;0HAcceleration Sensor Z: ");
    serialPrint("\e[11;0HJoystick X: ");
    serialPrint("\e[13;0HJOystick Y: ");
    serialPrint("\e[15;0HPotentiometer : ");
    serialPrint("\e[17;0HLDR: ");
    serialPrint("\e[19;0HNTC: ");
    serialPrint("\e[21;0HPB1: ");
    serialPrint("\e[23;0HPB2: ");
    serialPrint("\e[25;0HPB3: ");
    serialPrint("\e[27;0HPB4: ");
    serialPrint("\e[29;0HPB5: ");
    serialPrint("\e[31;0HPB6: ");

//serialPrint("\e[2;20H \e[K");
//serialPrint("\e[H");
//serialPrint("\e[2;20H\e[0K");
//serialPrintInt(5);

}

void disp_value()
{

    serialPrint("\e[H");
    serialPrint("\e[3;24H\e[0K ");
    if ((range <= 1)) // || (range > 15))
    {
        serialPrint("Out of Range");
    }
    else
    {
        serialPrintInt(range);
        serialPrint(" cm");
    }

    serialPrint("\e[5;24H\e[0K ");
    serialPrintInt(acc_values[0]);
    serialPrint(" m/s^2");

    serialPrint("\e[7;24H\e[0K ");
    serialPrintInt(acc_values[1]);
    serialPrint(" m/s^2");

    serialPrint("\e[9;24H\e[0K ");
    serialPrintInt(acc_values[2]);
    serialPrint(" m/s^2");

    serialPrint("\e[11;24H\e[0K ");
    serialPrintInt(adc_values[1]);

    serialPrint("\e[13;24H\e[0K ");
    serialPrintInt(adc_values[2]);

    serialPrint("\e[15;24H\e[0K ");
    serialPrintInt(pot);

    serialPrint("\e[17;24H\e[0K ");
    process_ldr();
//    serialPrintInt(ldr);

    serialPrint("\e[19;24H\e[0K ");
    serialPrintInt(ntc);

    serialPrint("\e[21;24H\e[0K ");
    serialPrintInt(pb & 0x01);

    serialPrint("\e[23;24H\e[0K ");
    serialPrintInt((pb >> 1) & 0x01);

    serialPrint("\e[25;24H\e[0K ");
    serialPrintInt((pb >> 2) & 0x01);

    serialPrint("\e[27;24H\e[0K ");
    serialPrintInt((pb >> 3) & 0x01);

    serialPrint("\e[29;24H\e[0K ");
    serialPrintInt((pb >> 4) & 0x01);

    serialPrint("\e[31;24H\e[0K ");
    serialPrintInt((pb >> 5) & 0x01);

    serialPrint("\e[33;0H\e[2KTo control - LED, LCD, RELAYS. Use below Format");
    serialPrint(
            "\e[34;0H\e[2KCommand Format - [Device] [Sub-Device] [Command] [Sub-Command]");
    serialPrint("\e[35;0H\e[2K");

    serialPrint("\e[37;0H\e[2KEnter Command :");
//    serialPrint("\e[3;0H? ");
    serialPrint("\e[38;0H\e[2K");

}

void get_user_input()
{

    while (serialAvailable() == 1)
    {
        if (serialAvailable() == 1)
        {
            char c = serialRead();
            input_cmd[index] = c;

            // For Backspace and Delete chars
            if ((c == 0x7F) || (c == 0x08))
            {
                input_cmd[index] = NULL;
                index--;
                input_cmd[index] = NULL;
                index--;
            }

            // For Carriage return or enter ie '\r'
            if (c == 0x0D)
            {
                memset(cmd_stored, NULL, 64);
                strcpy(cmd_stored, input_cmd);
                serialPrint("\e[1B\e[0E\e[2KCommand Entered: ");
                serialPrint(cmd_stored);
                index = 0;
                memset(input_cmd, NULL, 64);
                process_flag = 1;
                break;

            }

            index++;

        }
    }
}

void process_ldr()
{
    if (ldr <= 100)
        serialPrint("Low Intensity");
    if ((ldr > 100) && (ldr <= 400))
        serialPrint("Medium Intensity");
    if ((ldr > 450))
        serialPrint("High Intensity");
}

void get_sensor_readings()
{

    refresh_timer_stop();

    get_us_dist();
    // Actual as per dataset 58 but calibrated for the sensor on board
    range = distance / 52;

    uart_disable();

    get_joystick();

    get_acceleration();

    delay_ms(100);

    free_bus();

    // I2C disable
    P3OUT &= ~BIT3;

    uart_init();

    sensor_init();
    ntc = get_ntc();
    ldr = get_ldr();

    pot = get_pot();
    pb = get_pb();

    refresh_timer_start();

}

void reset_actuators()
{

    // relay off
    P3DIR |= BIT4;
    P3OUT &= ~BIT4;

    // shift leds off
    int a;
    for (a = 0; a <= 5; a++)
        led_array[a] = 0;
    shift_register_init();

    // leds green and red
    P1DIR |= BIT5;
    P1OUT &= ~BIT5;

    P3DIR |= BIT7;
    P3OUT &= ~BIT7;

    lcd_init();
    lcd_clear();
    delay_ms(30);

}

int main(void)
{
    initMSP();
    memset(input_cmd, NULL, 64);
    memset(cmd_stored, NULL, 64);
    memset(check, NULL, 3);

    uart_init();

    index = 0;
    dboard_flag = 0;

    while (1)
    {

        if (init_dash == 0)
        {
            serialPrint("\e[2J\e[H");
            serialPrint("\e[1;0HEnter sensorDashboard to view data");
            serialPrint("\e[1B\e[0E");
            init_dash = 1;
        }
        if (dboard_flag == 0)
        {
            get_user_input();

            if (process_flag == 1)
            {
                serialPrint(cmd_stored);
                if ((strcmp(cmd_stored, "sensorDashboard\r") == 0)
                        && (dboard_flag == 0))
                {
                    dboard_flag = 1;
                    disp_init();
                    process_flag = 0;
                    reset_actuators();
                    // TODO reset actuators.
                }
                else
                {
                    serialPrint("\e[0EWrong Entry. Try again.");
                    serialPrint("\e[1B\e[0E");
                    process_flag = 0;
                }
                process_flag = 0;

            }
        }
        // First time when the user presses the sensorDashboard
        if (dboard_flag == 1)
        {
            dboard_flag = 2;
            disp_flag = 1;
            time_counter = 4;
            user_mode = 0;

        }
        if ((disp_flag == 1) && (time_counter == 4))
        {
            get_sensor_readings();

            uart_init();

            disp_value();

            user_mode = 1;

        }
        if ((disp_flag == 1) && (user_mode == 1))
        {
            uart_init();

            get_user_input();
            if (process_flag == 1)
            {
                process_command();
                process_flag = 0;
                user_mode = 0;
            }
        }

        if (exit_dash == 1)
        {
            // Start over again
            exit_dash = 0;
            init_dash = 0;
            disp_flag = 0;
            dboard_flag = 0;
            user_mode = 0;
            reset_actuators();
            serialPrint("\e[0E\e[2KBoard Resetting...");
            serialPrint("\e[0EBoard Reset");
            serialPrint("\e[0EExit");
            delay_ms(5000);
        }

    }

}

# pragma vector = PORT1_VECTOR
__interrupt
void Port_1(void)
{
// Toggle to get the signal value
    TA0CCTL0 ^= CCIS0;

// Pullup disabled for P1.0
    P1REN &= ~BIT0;
    P1OUT &= ~BIT0;

//disable interrupt
    P1IES &= ~BIT0;
    P1IE &= ~BIT0;

// Stop Timer
    TA0CTL &= ~MC_2;

// Get the time
    distance = TA0CCR0;

// clear flag
    P1IFG &= ~BIT0;

}

#pragma vector = TIMER1_A0_VECTOR
__interrupt
void Timer(void)
{
    time_counter++;
    if (time_counter > 4)
    {
        //serialPrint("Interupt");
        //refresh_timer_stop();
        time_counter = 0;
    }
    TA1CCTL0 &= ~CCIFG;
}
