#include <p32xxxx.h>
#include "display.h"
#include "measure.h"
#include "music.h"
#include "gameplay.h"



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
    showInterface();
    // screenClear();
    // delay(100);
    // configurePWM();
    // configureT4();
    initRects();

    uchar i = 0;
    uchar j = 0;
    uchar step = 2;
    extern uchar combo;
    combo = 0;
    //uchar hits=0;
    ADC_init();
    double the_volt;
    while (1) {
        the_volt = 0;
        the_volt=getPressure();
        /*if (the_volt>1){
            pts[2].t=1;
            //clearRectangle(pts[2].x, pts[2].y, pts[2].x+BLOCK_WIDTH, pts[2].y+BLOCK_HEIGHTH,0);
        }else{
            pts[2].t=0;
            clearRectangle(pts[2].x, pts[2].y, pts[2].x+BLOCK_WIDTH, pts[2].y+BLOCK_HEIGHTH,0);
        }*/
        pts[2] = checkHits(pts[2], 56, 52, 0.0, 1.0, the_volt);
        pts[1] = checkHits(pts[1], 34, 52, 0.0, 1.0, the_volt);
        pts[0] = checkHits(pts[0], 11, 52, 0.0, 2.8, the_volt);
        pts[3] = checkHits(pts[3], 79, 52, 0.0, 2.8, the_volt);
        for (i = 0; i < 4; i++) {
                //moveRectangleDown(pts[i].x, pts[i].y, pts[i].x+BLOCK_WIDTH, pts[i].y+BLOCK_HEIGHTH, 3, 0);
                moveRectDown(pts[i], step, 0);
                pts[i].y = pts[i].y + step;
            
                if (pts[i].y >= 53) {
                    clearRectangle(pts[i].x, pts[i].y, pts[i].x+BLOCK_WIDTH, pts[i].y+BLOCK_HEIGHTH, 0);
                    pts[i].y = 1;
                    if (combo > 0 && pts[i].t == 1) comboClear();
                    pts[i].t = 1;
                    //showNumber(104, 45, j);
                }
                //showNumber(104,45,(char)(10*the_volt));
            delay(2);
        }
       /* the_volt=getPressure();
        if(the_volt<1){
            for(i=0;i<4;i++){
                Check_Remove_Rect(pts[i].x,pts[i].y,11,48);
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
        }else if (the_volt<2.0){
            for(i=0;i<4;i++){
                Check_Remove_Rect(pts[i].x,pts[i].y,34,48);
            }
            //drawCross(34, 58);
            delay(5);
        }else{
            
            drawCross(56, 58);
            setColorBoard(0);
            drawCross(34, 58);
            drawCross(11, 58);
            setColorBoard(1);
            
           for(i=0;i<4;i++){
                Check_Remove_Rect(pts[i].x,pts[i].y,56,48);
            }
            delay(5);
        }*/
        //showNumber(104,45,(char)(10*the_volt));
    }
    /*
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
    */

    return 0;
}
