#include <p32xxxx.h>
#include "display.h"

void MCU_init() {
    /* setup I/O ports to connect to the LCD module */
    // let A,B,D,E all to be output
    TRISD = 0xFFF0;
    PORTD = 0;
    TRISECLR = 0xFFFF;
    TRISACLR = 0xFFFF;
    TRISBCLR = 0xFFFF;

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
    IPC2SET = 0x000d;           // timer 2: priority is 3, subpriority is 1
    IFS0CLR = 0x0110;           // clear the flags for timer 1 and timer 2
    
    /* enable global and individual interrupts */
    asm( "ei" );                // enable interrupt globally
    // LATDbits.LATD2 = 1;
    IEC0SET = 0x0110;           // enable interrupt for timer 1 and timer 2
}

int main() {
    MCU_init();
    UART_init();

    screenClear();
    delay(200);
    showInit();
    delay(200);
    screenClear();
    delay(100);
    const unsigned char command_9[25] = {
        0xfe, 0xfd, 0x24, 0x00, 0x10, 
        0x00, 0x02, 0x00, 0x3c, 
        0x00, 0x0f, 0x00, 0x22, 
        0x00, 0x1d, 0x00, 0x3c, 
        0x00, 0x28, 0x00, 0x22, 
        0xdd, 0xcc, 0xbb, 0xaa
    };
    const unsigned char test[21] = {
        0xfe, 0xfd,
        0x23, 0x00, 0x0c, 
        0x00, 0x05, 0x00, 0x20,
        0xdd, 0xcc, 0xbb, 0xaa
    };
    // SendString(command_9, 25);
    SendString(test, 13);
    // SendString(test, 25);
    // screenClear();
    drawLine(0x04, 0x04, 0x54, 0x04, 1, 2);
    drawRectangle(0x6f, 0x00, 0x7f, 0x0f, 0);
    drawCross(0x20,0x20);
    delay(100);
    drawPoint(0x30,0x21);
    while (1) ;
    return 0;
}