#include <p32xxxx.h>
#include "display.h"

static const unsigned char COMMAND_BEGIN[2] = {0xfe, 0xfd};
static const unsigned char COMMAND_END[4] = {0xdd, 0xcc, 0xbb, 0xaa};

static const char _YLIM_ = 0x3f;
static const char _XLIM_ = 0x7f;
static uchar ifFrontColor = 1;

void UART_init(void) {
    asm("di");
    
    // The configuration will override PORTx config

    // Setup for UART Transmit
    U1MODEbits.BRGH = 1; // high speed
    U1BRG = 16; // baud rate: PBCLK / 4 / 9
    // The most common data format is 8,N,1 
    U1MODEbits.PDSEL = 0b00; // 8-bit data, no parity (means check)
    U1MODEbits.STSEL = 0; // 1-bit stop bit
    U1MODEbits.UEN = 0b00; // flow control not used
    // no inter-transmit interrupted are needed, skip
    U1STAbits.UTXEN = 1; // transimitter enabled, IF cleared
    IFS0bits.U1ATXIF = 0; // IF clear

    // config the UART Recieve
    // baud rate, stop bit are already configed;
    IFS0bits.U1ARXIF = 0;
    IEC0bits.U1ARXIE = 1; // set receive interrupt
    IEC0bits.U1RXIE = 1; // set receive interrupt
    IFS0bits.U1RXIF = 0;
    IPC6bits.U1AIP = 6; // set ipl6
    IPC6bits.U1AIS = 0b11; // sub priority 3
    U1STAbits.URXEN = 1; // reciver enabled 

    U1MODEbits.ON = 0; // reset then enable
    U1MODEbits.ON = 1;

    asm("ei");
    delay(500);
}

void SendData(const unsigned char dat){
	while (U1STAbits.TRMT!=1);
	U1TXREG = dat;
	while (U1STAbits.TRMT!=1);
}

void SendString(const unsigned char *s, unsigned int size){
	unsigned int i;
	for(i = 0; i < size; i++){
		U1TXREG = *s;
		while (U1STAbits.TRMT!=1);
		s++;
	}
}

/**
 * @brief Set the color to write
 * 
 * @param normal 0 if use Background color, use Frontground color otherwise
 */
void setColorBoard(uchar normal) {
    // normal is non-zero -> normal
    unsigned char set[5] = {0x21, 0x00, 0x02, 0x00, 0x0f};
    SendString(COMMAND_BEGIN, 2);
    if (normal == 0) {
        SendString(set, 5);
    } else {
        set[3] = 0x0f;
        set[4] = 0x00;
        SendString(set, 5);
    }
    SendString(COMMAND_END, 4);
}

void revertColor(void) {
    ifFrontColor = 1 - ifFrontColor;
    setColorBoard(ifFrontColor);
}

/**
 * @brief For debug, draw example from the documents
 * 
 */
void drawExample(void) {
    unsigned char example[15] = {
        0x33,0x00,0x0c,0x00,0x10,0x00,0x14,0xbc,0xaa,0xc8,0xf3,0xca,0xb5,0xd2,0xb5
    };
    SendString(COMMAND_BEGIN, 2);
    SendString(example, 15);
    SendString(COMMAND_END, 4);
}

/**
 * @brief Delay some time(~1ms) by CPU
 * @attention For debug, should not be used 
 */
void delaySome(void)	{
	unsigned char i, j;
	i = 2;
	j = 199;
	do {
		while (--j);
	} while (--i);
}

/**
 * @brief Delay time*1ms by CPU
 * @attention For debug, should not be used 
 * @param time the exact delay time 
 */
void delay(unsigned int time){
	unsigned int i;
	for(i = 0; i < time; i++){
		delaySome();
	}
}

void drawLineCommand(unsigned char * line_cmd, unsigned char size) {
    SendString(COMMAND_BEGIN, 2);
    SendString(line_cmd, size);
    SendString(COMMAND_END, 4);
}

/**
 * @brief draw line on the screen
 * @attention if draw horizontal or vertical, x1<x2, y1<y2
 * 
 * @param x1 x-axis of start point
 * @param y1 y-axis of start point
 * @param x2 x-axis of end point
 * @param y2 y-axis of end point
 * @param dashed non-zero if use dashed line, only support vertical or horizontal
 * @param sep the distance between two solid segments, only valid if dashed
 * @exception none, if point is outside the screen, do nothing
 */
void drawLine(uchar x1, uchar y1, uchar x2, uchar y2, uchar dashed, uchar sep) {
    if (x1 > _XLIM_ || x2 > _XLIM_ || y1 > _YLIM_ || y2 > _YLIM_) return;
    uchar line[19] = {0};
    line[0] = 0x24; line[2] = 0x10;
    line[4] = x1; line[6] = y1;
    line[8] = x2; line[10] = y2;
    line[12] = x1; line[14] = y1;
    line[16] = x2; line[18] = y2;
    if (dashed==0) {
        drawLineCommand(line, 19);
        return;
    }
    if (sep == 0) sep = 126;
    uchar start;
    start = y1;
    uchar end;
    end = y1 + sep;
    if (x1 == x2) {
        // horizontal
        while (end <= y2) {
            drawLine(x1, start, x1, end, 0, 0);
            revertColor();
            start = end;
            end += sep;
        }
        drawLine(x1, start, x1, y2, 0, 0);
    } 
    if (y1 == y2) {
        // vertical
        start = x1; end = x1;
        while (end <= x2) {
            drawLine(start, y1, end, y1, 0, 0);
            revertColor();
            start = end;
            end += sep;
        }
        drawLine(start, y1, x2, y1, 0, 0);
    }
    ifFrontColor = 1;
    setColorBoard(1);
}

void screenClear(void) {
    SendString(COMMAND_BEGIN, 2);
    unsigned char clear[3] = {0x22, 0x00, 0x00};
    SendString(clear, 3);
    SendString(COMMAND_END, 4);
}

uchar isInBoard(uchar x, uchar y){
    return (x < _XLIM_ && y < _YLIM_);
}

void drawRectangleCommand(uchar * rect_cmd, uchar size){
    SendString(COMMAND_BEGIN, 2);
    SendString(rect_cmd, 12);
    SendString(COMMAND_END, 4);
}

/**
 * @brief draw rectangle on the screen
 * 
 * @param x1 x-axis of start point
 * @param y1 y-axis of start point
 * @param x2 x-axis of end point
 * @param y2 y-axis of end point
 * @param solid non-zero if draw solid rectangle
 * @exception none, if point is outside the screen, do nothing
 */
void drawRectangle(uchar x1, uchar y1, uchar x2, uchar y2, uchar solid){
    if (x1 > _XLIM_ || x2 > _XLIM_ || y1 > _YLIM_ || y2 > _YLIM_) return;
    uchar rect[12] = {0};
    rect[0] = 0x26; rect[2] = 0x09;
    rect[5] = x1; rect[7] = y1;
    rect[9] = x2; rect[11] = y2;
    if (solid!=0) rect[3] = 0x01;
    drawRectangleCommand(rect, 12);
}

void clearRectangle(uchar x1, uchar y1, uchar x2, uchar y2, uchar solid){
    revertColor();
    drawRectangle(x1, y1, x2, y2, solid);
    revertColor();
}

/**
 * @brief draw point ad (x,y)
 * @attention have some bugs...
 * @exception if (x,y) out of the borad, do nothing
 * 
 * @param x 
 * @param y 
 */
void drawPoint(uchar x, uchar y){
    if (isInBoard(x,y)==0) return;
    uchar point[7] = {0};
    point[0] = 0x23; point[2] = 0x0c;
    point[4] = x; point[6] = y;
    SendString(COMMAND_BEGIN, 2);
    SendString(point, 7);
    SendString(COMMAND_END, 4);
}

/**
 * @brief draw a "+" at (x,y)
 * 
 * @param x x-axis
 * @param y y-axis 
 */
void drawCross(uchar x, uchar y) {
    drawLine(x, y-1, x, y+1,0,0);
    drawLine(x-1, y, x+1, y,0,0);
}

void showStart(void) {
    SendString(COMMAND_BEGIN, 2);
    uchar init[7] = {0};
    init[0] = 0x32; init[2] = 0x0c;
    init[4] = 0x08; init[6] = 0x18;
    SendString(init, 7);
    SendString("click to", 8);
    SendString(COMMAND_END, 4);
    delay(10);
    init[4] = 0x48;
    SendString(COMMAND_BEGIN, 2);
    SendString(init, 7);
    SendString(" start  ", 8);
    SendString(COMMAND_END, 4);
}

void showInit(void) {
    SendString(COMMAND_BEGIN, 2);
    uchar init[7] = {0};
    init[0] = 0x33; init[2] = 0x0c;
    init[4] = 0x18; init[6] = 0x12;
    SendString(init, 7);
    SendString("Loading", 8);
    SendString(COMMAND_END, 4);
}

void showEnd(void) {
    SendString(COMMAND_BEGIN, 2);
    uchar init[7] = {0};
    init[0] = 0x33; init[2] = 0x0c;
    init[4] = 0x02; init[6] = 0x12;
    SendString(init, 7);
    SendString("End....", 8);
    T4CONbits.TON = 0;
    T3CONbits.TON = 0;
    OC1CONbits.ON = 0; 
    SendString(COMMAND_END, 4);
    delay(1000);
}

/**
 * @brief move the Rectangle by "step"
 * @attention assume the rectangle is already here
 * 
 * @param x1 x-axis of the left upper point
 * @param y1 y-axis of the left upper point
 * @param x2 x-axis of the right lower point
 * @param y2 y-axis of the right lower point
 * @param step move step, usually 1 or 2
 * @param solid whether the rectangle to be moved is solid
 */
void moveRectangleDown(uchar x1, uchar y1, uchar x2, uchar y2, uchar step, uchar solid) {
    if (step > y2-y1) step = y2-y1;
    setColorBoard(0);
    drawRectangle(x1, y1, x2, y2, solid);
    // drawRectangle(x1, y1, x2, y1+step, solid);
    // drawLine(x1, y2, x2, y2, 0, 0);
    setColorBoard(1);
    drawRectangle(x1, y1+step, x2, y2+step, solid);
    // drawRectangle(x1, y1+step, x2, y2+step, solid);
}
void moveRectDown(pt a,uchar step, uchar solid){
    if(a.t==0){
        return;
    }else {
        moveRectangleDown(a.x,a.y,a.x+BLOCK_WIDTH,a.y+BLOCK_HEIGHTH,step,solid);
    }
}

/**
 * @brief show the char at (x,y)
 * @exception none, if (x,y) is outsize the board, do nothing
 * 
 * @param x x-axis
 * @param y y-aix
 * @param str char to show
 */
void showChar(uchar x, uchar y, uchar str) {
    if (isInBoard(x,y)==0) return;
    uchar word[7] = {0};
    word[0] = 0x31; word[2] = 0x05;
    word[4] = x; word[6] = y;
    SendString(COMMAND_BEGIN, 2);
    SendString(word, 7);
    SendData(str);
    SendString(COMMAND_END, 4);
}

/**
 * @brief show the two digits of num
 * 
 * @param x start x-axis
 * @param y start y-axis
 * @param num number to show
 */
void showNumber(uchar x, uchar y, uchar num) {
    if (isInBoard(x,y)==0) return;
    uchar a;
    a = (num % 100) / 10;
    showChar(x, y, a+48);
    a = num%10;
    showChar(x+6, y, a+48);
}

void initRects(void) {
    pts[0].x = 6;
    pts[1].x = 28;
    pts[2].x = 51;
    pts[3].x = 73;
    pts[0].y = 36;
    pts[1].y = 27;
    pts[2].y = 18;
    pts[3].y = 9;
    pts[0].t = 1;
    pts[1].t = 1;
    pts[2].t = 1;
    pts[3].t = 1;
}