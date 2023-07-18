/************************************************************************
* LCD.c
************************************************************************/
#include "LCD.h"

/* main driver */

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
    LATDbits.LATD1 = 1;
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

void LCD_put(const uchar c) {
    static word_count = 0;
    RS = 1;
    LCD_putchar(c);
    DelayMsec(30);
    RS = 0;
    word_count++;
    if (word_count==16) {
        LCD_init();
        word_count = 0;
    }
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
    int i;
    for (i=0; i < 15; i++);
    // PR2 = num;
    // flags.timer2_done = 0;
    // T2CONSET = 0x8000;              // turn on the timer 2
    // TMR2 = 0x0;                     // reset the timer 2
    // while ( ! flags.timer2_done );  // loop until flag 04 (for timer 1) is set
    // flags.timer2_done = 0;          // reset the flags
    // T2CONCLR = 0x8000;                    // turn off timer 
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
    int i;
    for (i=0; i < 15000; i++);
    // uchar i;
    // for (i=0; i<num; i++) {
    //    GenMsec();
    //}
}

/* timer 1 interrupt handler */ 
#pragma interrupt timer_2_interrupt ipl4 vector 8
void timer_2_interrupt(void) {
    T2CONCLR = 0x8000;                // turn off the timer 2
    IFS0CLR = 0x0110;           // clear the flag for timer 2
    flags.timer2_done = 1;      // set up the flags
    TMR2 = 0;
}

/*************end of LCD.c**************/
