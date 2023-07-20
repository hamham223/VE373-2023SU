#include <p32xxxx.h>
#include "display.h"
#include "measure.h"
#include "music.h"


void MCU_init() {
    /* setup I/O ports to connect to the LCD module */
    // let A,B,D,E all to be output
    TRISD = 0xFFF0;
    PORTD = 0;
    TRISECLR = 0xFFFF;
    TRISACLR = 0xFFFF;
    // TRISBCLR = 0xFFFF;

    /* setup Timer to count for 1 us and 1 ms */
    SYSKEY = 0x0;               // Ensure OSCCON is lock
    SYSKEY = 0xAA996655;        // Unlock sequence part 1 of 2 back to back instructions.
    SYSKEY = 0x556699AA;        // Unlock sequence part 2 of 2 back to back instructions.
    OSCCONbits.NOSC = 0x0007;   // Write new osc src value to NOSC control bits -- FRS, with original freqeuncy as 8 MHz
    OSCCONbits.FRCDIV = 0x0; // the prescale of FRC is 1
    OSCCONbits.PBDIV = 0x0;    // PBCLK is SYSCLK divided by 1. {(Not changed here)Clock is multiplied by 15. PLL output is divided by 1} -- PBCLK has freqeuncy 1 MHz
    OSCCONbits.OSWEN = 0x0001;  // Initiate clock switch by setting OSWEN bit.
    SYSKEY = 0x0;               // Write non-key value to perform a re-lock.

    while(OSCCONbits.OSWEN);    // Loop until OSWEN = 0. Value of 0 indicates osc switch is complete.

    /* Configure Timer interrupts */ 
    INTCONbits.MVEC = 1;        // multi-vector mode
    IEC0SET = (1 << 10);   // ?? Timer2 ??
    IEC0SET = (1 << 12);   // ?? Timer3 ??
    IEC0SET = (1 << 14);
    
    /* enable global and individual interrupts */
    asm( "ei" );                // enable interrupt globally
    // LATDbits.LATD2 = 1;
}

int main() {
    MCU_init();
    UART_init();
    // drawExample();
    showInit();
    delay(50);
    // screenClear();
    // delay(100);
    configurePWM();
    configureT4();

    drawLine(90, 0, 90, 63, 0, 0);
    drawLine(22, 0, 22, 63, 1, 2);
    drawLine(45, 0, 45, 63, 1, 2);
    drawLine(67, 0, 67, 63, 1, 2);
    drawRectangle(0x00, 0x00, 0x7f, 0x3f, 0);

    drawCross(11, 58);
    drawCross(34, 58);
    drawCross(56, 58);
    drawCross(79, 58);

    drawRectangle(98, 6, 118, 21, 0);
    showChar(106, 9, 'H');
    drawRectangle(98, 24, 118, 39, 0);
    showChar(104, 27, '9');
    showChar(110, 27, '9');
    drawRectangle(98, 42, 118, 57, 0);
    showChar(104, 45, '9');
    showChar(110, 45, '9');
    initRects();

    uchar i = 0;
    uchar j = 0;
    ADC_init();
    double the_volt;
    while (1) {
        the_volt = 0;
        for (i = 0; i < 4; i++) {
            moveRectangleDown(pts[i].x, pts[i].y, pts[i].x+BLOCK_WIDTH, pts[i].y+BLOCK_HEIGHTH, 3, 0);
            pts[i].y = pts[i].y + 3;
            if (pts[i].y >=47) {
                clearRectangle(pts[i].x, pts[i].y, pts[i].x+BLOCK_WIDTH, pts[i].y+BLOCK_HEIGHTH, 0);
                pts[i].y = 1;
                j = j+1;
                showNumber(104, 45, j);
            }
            delay(4);
        }
        the_volt=getPressure();
        if(the_volt<1){
            drawCross(11, 58);
            delay(5);
        }else if (the_volt<2.0){
            drawCross(34, 58);
            delay(5);
        }else{
            drawCross(56, 58);
            setColorBoard(0);
            drawCross(34, 58);
            drawCross(11, 58);
            setColorBoard(1);
            delay(5);
        }
        showNumber(104,45,(char)(10*the_volt));
        delay(5);
    }

    
    ADC_init();
    while(1){
        the_volt=getPressure();
        if(the_volt<1){
            drawCross(11, 58);
            delay(10);
        }else if (the_volt<2.0){
            drawCross(34, 58);
            delay(10);
        }else{
            drawCross(56, 58);
            setColorBoard(0);
            drawCross(34, 58);
            drawCross(11, 58);
            setColorBoard(1);
            delay(10);
        }
        showNumber(104,45,(char)(10*the_volt));
        delay(20);

    }
    
    return 0;
}