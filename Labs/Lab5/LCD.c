// DEVCFG3
// USERID = No Setting
#pragma config FSRSSEL = PRIORITY_0     // SRS Select (SRS Priority 0)
#pragma config FCANIO = OFF             // CAN I/O Pin Select (Alternate CAN I/O)
#pragma config FUSBIDIO = OFF           // USB USID Selection (Controlled by Port Function)
#pragma config FVBUSONIO = OFF          // USB VBUS ON Selection (Controlled by Port Function)

// DEVCFG2
#pragma config FPLLIDIV = DIV_2         // PLL Input Divider (2x Divider)
#pragma config FPLLMUL = MUL_15         // PLL Multiplier (15x Multiplier)
#pragma config UPLLIDIV = DIV_1         // USB PLL Input Divider (1x Divider)
#pragma config UPLLEN = OFF             // USB PLL Enable (Disabled and Bypassed)
#pragma config FPLLODIV = DIV_1         // System PLL Output Clock Divider (PLL Divide by 1)

// DEVCFG1
#pragma config FNOSC = PRIPLL           // Oscillatoits (Primary Osc w/PLL (XT+,HS+,EC+PLL))
#pragma config FSOSCEN = ON            // Secondary Oscillator Enable (Enabled)
#pragma config IESO = ON               // Internal/External Switch Over (Enabled)
#pragma config POSCMOD = HS             // Primary Oscillator Configuration (HS osc mode)
#pragma config OSCIOFNC = ON            // CLKO Output Signal Active on the OSCO Pin (Enabled)
#pragma config FPBDIV = DIV_2           // Peripheral Clock Divisor (Pb_Clk is Sys_Clk/2)
#pragma config FCKSM = CSECMD           // Clock Switching and Monitor Selection (Clock Switch Enable, FSCM Disabled)
#pragma config WDTPS = PS1              // Watchdog Timer Postscaler (1:1)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable (WDT Disabled (SWDTEN Bit Controls))

// DEVCFG0
#pragma config DEBUG = OFF              // Background Debugger Enable (Debugger is disabled)
#pragma config ICESEL = ICS_PGx2        // ICE/ICD Comm Channel Select (ICE EMUC2/EMUD2 pins shared with PGC2/PGD2)
#pragma config PWP = OFF                // Program Flash Write Protect (Disable)
#pragma config BWP = OFF                // Boot Flash Write Protect bit (Protection Disabled)
#pragma config CP = OFF                 // Code Protect (Protection Disabled)

/************************************************************************
* LCD.c
************************************************************************/
#include "LCD.h"
//#include <plib.h>

/* initialize the LCD module */
void LCD_init() {
    DelayMsec(16);      //wait for 16 ms(over 15ms)
    RS = 0;             //send command
    E = 1;
    Data = LCD_IDLE;    //function set - 8 bit interface
    E = 0;
    DelayMsec(5);       //wait for 5 ms
    E = 1;
    Data = LCD_IDLE;    //function set - 8 bit interface
    E = 0;
    DelayUsec(200);     //wait for 200 us(over 100us)
    E = 1;
    Data = LCD_IDLE;    //function set
    E = 0;
    DelayMsec(5);
    E = 1;
    Data = 0b00100000; //function set - 4 bit interface
    E = 0;
    LCD_putchar(LCD_2_LINE_4_BITS);
    DelayMsec(100);
    LCD_putchar(LCD_DSP_CSR);
    DelayUsec(40);
    LCD_putchar(LCD_CLR_DSP);
    DelayMsec(5);
    LCD_putchar(LCD_CSR_INC);
}

/* Send one byte c (instruction or data) to the LCD */
void LCD_putchar(uchar c) {
    E = 1;
    Data = c;   //sending higher nibble
    E = 0;      //producing falling edge on E
    E = 1;
    Data <<= 4; //sending lower nibble through higher 4 ports
    E = 0;      //producing falling edge on E
}

/* Display a string of characters *s by continuously calling LCD_putchar() */
void LCD_puts(const uchar *s, int length) {
    // ...your code goes here
    // ...NOTE: must wait for at least 40 us after sending each character to
        // the LCD module.
    RS = 1;
    int i;
    for (i = 0; i < length; i++) {
        LCD_putchar(s[i]);
        DelayMsec(30);
    }
    RS = 0;
}

/* go to a specific DDRAM address addr */
void LCD_goto(uchar addr) {
    // ...send an address to the LCD
    // ...NOTE: must wait for at least 40 us after the address is sent
    // TODO;
    RS = 0;
    E = 1; Data = 0x80 + addr; E = 0;
    E = 1; Data <<=4;          E = 0;
    DelayUsec(40);
}

/* configure timer SFRs to generate num us delay*/
void DelayUsec(uchar num) {
    // ...your code goes here
    PR1 = num;
    flags.timer1_done = 0;
    T1CONSET = 0x8000;              // turn on the timer 1
    TMR1 = 0x0;                     // reset the timer 1
    while ( ! flags.timer1_done );  // loop until flag 04 (for timer 1) is set
    flags.timer1_done = 0;          // reset the flags
    T1CONCLR = 0x8000;                    // turn off timer 
}

/* configure timer SFRs to generate 1 ms delay*/
void GenMsec() {
    // ...your code goes here
    DelayUsec(250);
    DelayUsec(250);
    DelayUsec(250);
    DelayUsec(250);
}

/* Call GenMsec() num times to generate num ms delay*/
void DelayMsec(uchar num) {
    uchar i;
    for (i=0; i<num; i++) {
        GenMsec();
    }
}

/* timer 1 interrupt handler */ 
#pragma interrupt timer_1_interrupt ipl3 vector 4
void timer_1_interrupt(void) {
    T1CONCLR = 0x8000;                // turn off the timer 1
    IFS0CLR = 0x0010;           // clear the flag for timer 1
    flags.timer1_done = 1;      // set up the flags
    TMR1 = 0;
}

/*************end of LCD.c**************/
