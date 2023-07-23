#ifndef _DISPLAY_H
#define _DISPLAY_H

typedef unsigned char uchar;

typedef struct _pt_ {
    uchar x;
    uchar y;
    uchar t;
} pt;

void UART_init(void);

void SendData(const uchar dat);
void SendString(const unsigned char *s, unsigned int size);

void showInit(void);
void showEnd(void);
void showChar(uchar x, uchar y, uchar str);
void showNumber(uchar x, uchar y, uchar num);
void drawExample(void);
void drawLineCommand(unsigned char * line_cmd, unsigned char size);
void drawLine(uchar x1, uchar y1, uchar x2, uchar y2, uchar dashed, uchar sep);
void drawRectangleCommand(uchar * rect_cmd, uchar size);
void drawRectangle(uchar x1, uchar y1, uchar x2, uchar y2, uchar solid);
void clearRectangle(uchar x1, uchar y1, uchar x2, uchar y2, uchar solid);
void moveRectangleDown(uchar x1, uchar y1, uchar x2, uchar y2, uchar step, uchar solid);
void moveRectDown(pt a,uchar step, uchar solid);
void drawPoint(uchar x, uchar y);
void drawCross(uchar x, uchar y);

uchar isInBoard(uchar x, uchar y);
void setColorBoard(uchar normal);
void screenClear(void);

void delaySome(void);
void delay(unsigned int time);

void initRects(void);


// The matrix have the same shape
// 10*10, x = 10 ?
#define BLOCK_HEIGHTH 10
#define BLOCK_WIDTH 10
pt pts[4];


#endif /* Display.h */