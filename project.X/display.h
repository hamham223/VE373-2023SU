#ifndef _DISPLAY_H
#define _DISPLAY_H

typedef  unsigned char uchar;

void UART_init(void);

void SendData(const uchar dat);
void SendString(const unsigned char *s, unsigned int size);

void drawExample(void);
void drawLineCommand(unsigned char * line_cmd, unsigned char size);
void drawLine(uchar x1, uchar y1, uchar x2, uchar y2, uchar dashed, uchar sep);

void setColorBoard(uchar normal);
void screenClear(void);

void delaySome(void);
void delay(unsigned int time);

#endif /* Display.h */