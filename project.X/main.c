#include <p32xxxx.h>
#include <proc/p32mx795f512l.h>
#include "display.h"
#include "measure.h"
#include "music.h"
#include "gameplay.h"



void MCU_init() {
    /* setup I/O ports to connect to the LCD module */
    // let A,B,D,E all to be output

    PORTD = 0;
    TRISECLR = 0xFFFF;
    TRISACLR = 0xFFFF;
    TRISDCLR = 0xFFFF;
    TRISD = 0xFFF0;
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

static uchar gameStart = 0;
#define readD  PORTDbits.RD13
#pragma interrupt CN_ISR ipl5 vector 26
void CN_ISR (void) {
    LATDbits.LATD2 = 1;
	IEC1bits.CNIE = 0;
    gameStart = 1;
	int n = 0;
	while (n < 2000) n++;

	IFS1bits.CNIF = 0;
    IEC1bits.CNIE = 1;
}

int main() {
    MCU_init();
    UART2_init();
    UART_init();
    CN_init();
    ADC_init();
    SendString2("11243431", 9);

    screenClear();
    showStart();
    while (gameStart == 0);
    gameStart = 0;
    
    screenClear();
    showInit();
    delay(500);
    screenClear();
    showInterface();

    configurePWM();
    configureT4();
    initRects();

    uchar i = 0;
    uchar j = 0;
    uchar step = 2;
    extern uchar combo;
    combo = 0;
    extern uchar global_index;
    extern uchar global_count;
    
    double the_volt;
    while (global_count < 2) {
        the_volt = getPressure();

        pts[2] = checkHits(pts[2], 56, 52, 0.0, 3.3, the_volt);
        pts[1] = checkHits(pts[1], 34, 52, 0.0, 3.3, the_volt);
        pts[0] = checkHits(pts[0], 11, 52, 0.0, 3.3, the_volt);
        pts[3] = checkHits(pts[3], 79, 52, 0.0, 3.3, the_volt);
        
        if (global_index==11 || global_index==37){
                clearAll();
                initRects_p1();
        }
        if (global_index==21||global_index==1){
                clearAll();
                initRects();
        }
        if (global_index==55){
                clearAll();
                initRects_p2();
        }
        if (global_index==75){
                clearAll();
                initRects_p1();
        }
        if (global_index==65){
                clearAll();
                initRects_p3();
        }
        for (i = 0; i < 4; i++) {
                //moveRectangleDown(pts[i].x, pts[i].y, pts[i].x+BLOCK_WIDTH, pts[i].y+BLOCK_HEIGHTH, 3, 0);
                moveRectDown(pts[i], step, 0);
                pts[i].y = pts[i].y + step;
            
                if (pts[i].y >= 53) {
                    clearRectangle(pts[i].x, pts[i].y, pts[i].x+BLOCK_WIDTH, pts[i].y+BLOCK_HEIGHTH, 0);
                    pts[i].y = getInitHeight();
                    if (pts[i].t == 1) comboClear();
                    //pts[i].t = 1;
                }
            delay(2);
        }
    }
    showEnd();
    
    return 0;
}
