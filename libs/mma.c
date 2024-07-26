/***************************************************************************//**
 * @file    mma.c
 * @author  Amrutha Venkatesan
 * @date    1st June 2023
 * matriculation number - 5169728
 * email - amsvenkat20@gmail.com
 *
 *
 * @brief   source file for mma
 *
 * Code implements the sensor MMA8451Q.
 * Multiple helper functions added - set_standby_mode, set_active_mode, range_functions,
 ******************************************************************************/

#include "./mma.h"

/******************************************************************************
 * VARIABLES
 *****************************************************************************/
// Register addresses
#define MMA8451_SLAVE_ADDRESS (0x1D)
#define XYZ_DATA_CFG (0x0E)
#define CTRL_REG1 (0x2A)
#define CTRL_REG2 (0x2B)
#define F_SETUP (0x09)
#define OUT_X_MSB (0x01)
#define PULSE_CFG (0x21)
#define PULSE_SRC (0x22)
#define PULSE_THSZ (0x25)
#define PULSE_TMLT (0x26)
#define PULSE_LTCY (0x27)
#define PULSE_WIND (0x28)
#define CTRL_REG4 (0x2D)
#define CTRL_REG5 (0x2E)
#define CTRL_REG3 (0x2C)

// Counts per g for each of the scale and range
#define DIV_14_2G           4096
#define DIV_14_4G           2048
#define DIV_14_8G           1024
#define DIV_8_2G            64
#define DIV_8_4G            32
#define DIV_8_8G            16
#define GRAVITY    9.8

// All the bit values
#define RST (0x40)
#define G2 (0x0)
#define G4 (0x1)
#define G8 (0x2)
#define F_READ (0x2)
#define FS0 (0x01)
#define FS1 (0x02)
#define STANDBY (0xFE)
#define ACTIVE (0x01)
#define ST (0x80)



unsigned char CMD_CTRL_REG1 = 0x00;
unsigned char CMD_CTRL_REG2 = 0x00;
unsigned char CMD_XYZ_DATA_CFG = 0x00;
unsigned char CMD_PULSE_CFG = 0x00;
unsigned char CMD_CTRL_REG3 = 0x00;
unsigned char CMD_CTRL_REG4 = 0x00;
unsigned char CMD_CTRL_REG5 = 0x00;
unsigned char CMD_PULSE_LTCY = 0x00;
unsigned char CMD_PULSE_THSZ = 0x00;
unsigned char CMD_PULSE_TMLT = 0x00;
unsigned char CMD_PULSE_WIND = 0x00;


unsigned char xyz_values_8bit[3] = { 0, 0, 0 };
unsigned char xyz_values_14bit[6] = { 0, 0, 0, 0, 0, 0 };
int set_standby = 0;
int data_range = 0;
int data_resolution = 0;

/******************************************************************************
 * LOCAL FUNCTION PROTOTYPES
 *****************************************************************************/
void mma_write(unsigned char a, unsigned char b);
void set_standby_mode();
void set_active_mode();
int get_range(int r);
signed char mma_get8(unsigned char axis);
int mma_get14(unsigned char axis);
double mma_getreal(unsigned char axis);

/******************************************************************************
 * LOCAL FUNCTION IMPLEMENTATION
 *****************************************************************************/
void mma_write(unsigned char a, unsigned char b)
{
    unsigned char r[2] = { a, b };
    i2c_write(2, r, 1);

}

void set_standby_mode()
{

    CMD_CTRL_REG1 &= ~ACTIVE;
    unsigned char r[2] = {0};
    r[0] = CTRL_REG1;
    r[1] = CMD_CTRL_REG1;
    i2c_write(2, r, 1);
    //mma_write(CTRL_REG1,  CMD_CTRL_REG1);

    __delay_cycles(10000);
    set_standby = 1;
}

void set_active_mode()
{

    CMD_CTRL_REG1 |= ACTIVE;
    unsigned char r[2] = {0};
    r[0] = CTRL_REG1;
    r[1] = CMD_CTRL_REG1;
    i2c_write(2, r, 1);
    //mma_write(CTRL_REG1, CMD_CTRL_REG1);

    __delay_cycles(1000000);

    set_standby = 0;
}

int get_range(int r)
{
    int value;
    if (r == 14)
    {
        switch (data_range)
        {
        case 2:
            value = DIV_14_2G;
            break;

        case 4:
            value = DIV_14_4G;
            break;

        case 8:
            value = DIV_14_8G;
            break;
        }
    }
    else
    {
        switch (data_range)
        {
        case 2:
            value = DIV_8_2G;
            break;

        case 4:
            value = DIV_8_4G;
            break;

        case 8:
            value = DIV_8_8G;
            break;
        }
    }

    return value;
}

signed char mma_get8(unsigned char axis)
{

    signed char value;
    // index for msb14, msb8
    char a, b;

    if (axis == 'x')
    {
        a = 0;
        b = 0;
    }

    if (axis == 'y')
    {
        a = 2;
        b = 1;
    }

    if (axis == 'z')
    {
        a = 4;
        b = 2;
    }

    unsigned char rx[1] = { 0 };
    unsigned char r[1] = { CTRL_REG1 };

    i2c_write(1, r, 0);
    i2c_read(1,rx);

    if (!(rx[0] & F_READ ) )  //data_resolution == 14
    {
        value = xyz_values_14bit[a];
    }
    if (rx[0] & F_READ )
    {
        value = xyz_values_8bit[b];
    }

    if ((value >> 7)!= 0) //((value / 8192) == 1)
    {
        value = ~value + 1;
    }

    return value;
}

int mma_get14(unsigned char axis)
{

    signed char msb, lsb;
    // For msb14, lsb14, msb8
    char a, b, c;
    int value;

    if (axis == 'x')
    {
        a = 0;
        b = 1;
        c = 0;
    }
    if (axis == 'y')
    {
        a = 2;
        b = 3;
        c = 1;
    }

    if (axis == 'z')
    {
        a = 4;
        b = 5;
        c = 2;
    }

    unsigned char rx[1] = { 0 };
    unsigned char r[1] = { CTRL_REG1 };

    i2c_write(1, r, 0);
    i2c_read(1,rx);


    if (!(rx[0] & F_READ ) )  // data_resolution == 14
    {
        msb = xyz_values_14bit[a];
        lsb = xyz_values_14bit[b];
    }
    if (rx[0] & F_READ )  // data_resolution == 8
    {
        msb = xyz_values_8bit[c];
        lsb = 0x0;
    }

//    if (msb )

    value = (msb << 6) | (lsb >>2);


    if ((value >> 15)!=0) //((value / 8192) == 1)
    {
        value = ~value + 1;
    }

    return value;
}

double mma_getreal(unsigned char axis)
{
    double real;
    int curr_range;

    unsigned char rx[1] = { 0 };
    unsigned char r[1] = { CTRL_REG1 };

    i2c_write(1, r, 0);
    i2c_read(1,rx);

    if (rx[0] & F_READ )
    {
        if (axis == 'x')
            real = mma_get8X();
        if (axis == 'y')
            real = mma_get8Y();
        if (axis == 'z')
            real = mma_get8Z();

        curr_range = get_range(8);
    }

    if (!(rx[0] & F_READ ) )
    {
        if (axis == 'x')
            real = mma_get14X();
        if (axis == 'y')
            real = mma_get14Y();
        if (axis == 'z')
            real = mma_get14Z();

        curr_range = get_range(14);
    }

    real /= curr_range;
    return real * GRAVITY;
}

/******************************************************************************
 * FUNCTION IMPLEMENTATION
 *****************************************************************************/
/*
 *
 * 	Have fun! :-)
 *
 */

unsigned char mma_setRange(unsigned char range)
// measurement range. (0: 2g, 1: 4g, >1: 8g)
{
    // Make changes in XYZ_DATA_CFG - address - 0x0E
    // Value to the register -> FS1 FS0

    set_standby_mode();

    if (range == 0)
    {
        CMD_XYZ_DATA_CFG &= ~(FS0 | FS1);
        data_range = 2;
    }

    if (range == 1)
    {
        CMD_XYZ_DATA_CFG &= ~FS1;
        CMD_XYZ_DATA_CFG |= FS0;
        data_range = 4;
    }

    if (range > 1)
    {
        CMD_XYZ_DATA_CFG &= ~FS0;
        CMD_XYZ_DATA_CFG |= FS1;
        data_range = 8;
    }

    mma_write(XYZ_DATA_CFG, CMD_XYZ_DATA_CFG);

    //set_active_mode();

    return 0;
}

unsigned char mma_setResolution(unsigned char resolution)
// measurement range. (0: 8bit, >=1: 14bit)
{
    unsigned char value;
    // Make changes in CTRL_REG1 - address - 0x2A
    // Value to the register -> F_READ bit

    set_standby_mode();

    // F_READ bit = 1
    if (resolution == 0)
    {
        CMD_CTRL_REG1 |= F_READ ;
        data_resolution = 8;
    }

    // F_READ bit = 0
    if (resolution >= 1)
    {
        CMD_CTRL_REG1 &= ~F_READ;
        data_resolution = 14;
    }

    mma_write( CTRL_REG1, CMD_CTRL_REG1);


    //set_active_mode();

    return 0;
}

unsigned char mma_init(void)
{
    unsigned char addr = 0x1D;
    i2c_init(addr);

    set_standby_mode();

    CMD_CTRL_REG2 |= RST;
    mma_write(CTRL_REG2, CMD_CTRL_REG2);
    __delay_cycles(10000);

    unsigned char rx[1] = { 0 };
    unsigned char r[1] = { CTRL_REG2 };
    _delay_cycles(16000);

    // Wait for the RST bit to clear
    do
    {
        i2c_write(1, r, 0);
        i2c_read(1,rx);
        rx[0] &= RST;
    }
    while (rx[0]);

    // Set resolution to 8 Bit - 0
    mma_setResolution(0);

    // Set range to 4g
    mma_setRange(1);

    //set_active_mode();

    return 0;
}

unsigned char mma_selftest(void)
{
    int xbef,ybef ,zbef ,yaft ,zaft ;
    int diff_x , diff_y , diff_z ;
    char result, xaft;

    CMD_CTRL_REG2 = 0x00;
    CMD_CTRL_REG1 = 0x00;
    set_standby_mode();

    // Set range to 4g
    mma_setResolution(1);
    mma_setRange(1);

    CMD_CTRL_REG2 &= ~ST;
    mma_write(CTRL_REG2,  CMD_CTRL_REG2);
    __delay_cycles(10000);

    set_active_mode();
    mma_read();

    unsigned char rx[1] = { 0 };
    unsigned char r[1] = { CTRL_REG1 };

    i2c_write(1, r, 0);
    i2c_read(1,rx);

    if  (rx[0] & F_READ )
    {
        xbef = mma_get8X();
        ybef = mma_get8Y();
        zbef = mma_get8Z();
    }
    if  (!(rx[0] & F_READ ) )
    {
        xbef = mma_get14X();
        ybef = mma_get14Y();
        zbef = mma_get14Z();
    }

    // Self test enable
    set_standby_mode();

    CMD_CTRL_REG2 |= ST;
    mma_write(CTRL_REG2, CMD_CTRL_REG2);
    __delay_cycles(10000);

    i2c_write(1, r, 0);
    i2c_read(1,rx);

    set_active_mode();
    mma_read();

    __delay_cycles(10000);

    if (rx[0] & F_READ )
    {
        xaft = mma_get8X();
        yaft = mma_get8Y();
        zaft = mma_get8Z();
    }
    if (!(rx[0] & F_READ ) )
    {
        xaft = mma_get14X();
        yaft = mma_get14Y();
        zaft = mma_get14Z();
    }

    diff_x = xaft - xbef;
    diff_y = yaft - ybef;
    diff_z = zaft - zbef;

    // x - 181, y - 255, z - 1680 for 4G
    if ((diff_x > 150 && diff_x < 210) && (diff_y > 225 && diff_y < 285)
            && (diff_z > 1650 && diff_z < 1710) == 1)
        result = 0;
    else
        result = 1;

    // Self test disabled
    set_standby_mode();

    return result;
}

unsigned char mma_enableTapInterrupt(void)
{

    set_standby_mode();
    // Pulse config - ODR at 800Hz, 0110 0000  - ZDPEFE or may be 0010 0000 - 32 for without latch enabled
    mma_write(PULSE_CFG, 0x20);

    // Pulse threshold - at ODR 800 Hz ,32 counts - as 1 step is 0.063g/count and taking it for 2g - 0010 0000
    mma_write(PULSE_THSZ, 0x20);

    // Pulse timelimit - at 800Hz, without LPF in normal mode - 100ms is 160 counts for 0.625 per step per count
    mma_write(PULSE_TMLT, 0xA0);

    // Pulse Latency at 800Hz , without LPF in normal mode  - 200 ms is 160 counts for 1.25 ms per step per count
    mma_write(PULSE_LTCY, 0xA0);

    // Pulse window at 800 Hz, without LP and in normal mode - 90ms is 72 counts for 1.25 ms per step per count
    mma_write(PULSE_WIND, 0x48);

    // Enabling the bit INT_EN_PULSE for Interrupt
    mma_write(CTRL_REG4, 0x8);

    // Enabling the bit INT_CFG_PULSE for INT 1
    mma_write(CTRL_REG5, 0x8);

    // Interupt polarity
    mma_write(CTRL_REG3, 0x2);

    set_active_mode();
    return 0;

}

unsigned char mma_disableTapInterrupt(void)
{
    // Disabling the Interrupt pin
    mma_write(CTRL_REG4, 0x0);
    return 0;
}

unsigned char mma_read(void)
{
    //set_active_mode();

    // FMODE == 0
    unsigned char r[1] = { OUT_X_MSB };
    i2c_write(1, r, 0);

    // if F_READ ==1
    if (data_resolution == 8)
    {
        i2c_read(3, xyz_values_8bit);

    }
    // if F_READ ==0
    if (data_resolution == 14)
    {
        i2c_read(6, xyz_values_14bit);
    }

    // FMODE == 1?
    return 0;
}

signed char mma_get8X(void)
{
    return mma_get8('x');
}

signed char mma_get8Y(void)
{
    return mma_get8('y');
}

signed char mma_get8Z(void)
{
    return mma_get8('z');
}

int mma_get14X(void)
{
    return mma_get14('x');
}

int mma_get14Y(void)
{
    return mma_get14('y');
}

int mma_get14Z(void)
{
    return mma_get14('z');
}

double mma_getRealX(void)
{
    return mma_getreal('x');
}

double mma_getRealY(void)
{
    return mma_getreal('y');
}

double mma_getRealZ(void)
{
    return mma_getreal('z');
}

//1. 14-bit data
//2g (4096 counts/g = 0.25 mg/LSB) 4g (2048 counts/g = 0.5 mg/LSB) 8g (1024 counts/g = 1 mg/LSB)
//2. 8-bit data
//2g (64 counts/g = 15.6 mg/LSB) 4g (32 counts/g = 31.25 mg/LSB) 8g (16 counts/g = 62.5 mg/LSB)

