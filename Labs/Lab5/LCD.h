/************************************************************************
* LCD.h
* Header file for the LCD Driver
************************************************************************/
#ifndef LCD_H
#define LCD_H 1
#include <p32xxxx.h>

/* define macros for LCD instructions */
#define LCD_IDLE 0x33
#define LCD_2_LINE_4_BITS 0x28
#define LCD_2_LINE_8_BITS 0x38
#define LCD_DSP_CSR 0x0c 
#define LCD_CLR_DSP 0x01
#define LCD_CSR_INC 0x06
#define LCD_SFT_MOV 0x14

// * define macros for interfacing ports
#define RS      PORTDbits.RD0
#define E       PORTDbits.RD1
#define Data    PORTE

typedef unsigned char uchar;

struct bits {
    unsigned timer1_done : 1;
//    ...other user defined flags may go here
} flags;

// * define constant strings for display */
const uchar startStr1[] = "Digital Clock";
const uchar startStr2[] = "SJTU JI - LAB3";
const uchar startStr3[] = "BIRD FIRE";
const uchar startStr4[] = "Roselia";

/* Function prototypes */
void LCD_init(void);
void LCD_putchar(uchar c);
void LCD_puts(const uchar *s, int length);
void LCD_goto(uchar addr);
void GenMsec(void);
void DelayUsec(uchar num);
void DelayMsec(uchar num);

/*****************end of LCD.h**********************/
#endif LCD_H