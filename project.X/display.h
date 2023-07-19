#ifndef _DISPLAY_H
#define _DISPLAY_H

typedef  unsigned char uchar;

void UART_init(void);

void SendData(const uchar dat);
void SendString(const unsigned char *s, unsigned int size);

void showInit(void);
void drawExample(void);
void drawLineCommand(unsigned char * line_cmd, unsigned char size);
void drawLine(uchar x1, uchar y1, uchar x2, uchar y2, uchar dashed, uchar sep);
void drawRectangleCommand(uchar * rect_cmd, uchar size);
void drawRectangle(uchar x1, uchar y1, uchar x2, uchar y2, uchar solid);
void moveRectangleDown(uchar x1, uchar y1, uchar x2, uchar y2, uchar step, uchar solid);
void drawPoint(uchar x, uchar y);
void drawCross(uchar x, uchar y);

uchar isInBoard(uchar x, uchar y);
void setColorBoard(uchar normal);
void screenClear(void);

void delaySome(void);
void delay(unsigned int time);

#endif /* Display.h */