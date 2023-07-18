#include <p32xxxx.h>
#define DELAY 14000  //??? msec

main() {
// 0. initialization
    DDPCONbits.JTAGEN = 0; //disable JTAGport, free up PORTA
    TRISD = 0xfff0; //configure inputs and outputs
    PORTD = 0; //clear PORTD

    while(1) {
        LATDbits.LATD0 = 1; //flash LEDs
        LATDbits.LATD1 = 1;
        delay_half_sec(1);

        LATDbits.LATD0 = 0;
        LATDbits.LATD1 = 0;
        delay_half_sec(1);
        LATDbits.LATD0 = 0;
        LATDbits.LATD1 = 1;
        delay_half_sec(1);
        LATDbits.LATD0 = 0;
        LATDbits.LATD1 = 0;
        delay_half_sec(1);
        if (PORTDbits.RD6==0) { //if SWl pressed
            delay_half_sec(3); //debounce
            if (PORTDbits.RD6==0) while(1); //trap
        } 
    }
}

delay_half_sec(int number) {
    int i;
    for (i=0; i < DELAY*number; i++);
}
