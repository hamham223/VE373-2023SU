#include <p32xxxx.h>
#include "LCD.h"

#define readD PORTDbits.RD13

char message = '6';
int displayed = 1;

#pragma interrupt UART_receive ipl6 vector 24
void UART_receive(void) {
    // receive a character
    IEC0bits.U1RXIE = 0;
    IEC0bits.U1ARXIE = 0;
    LATDbits.LATD2 = 1;
    uchar rx;
    while (U1STAbits.URXDA) {
        rx = U1RXREG;
    }
    LCD_put(rx);
    
    IFS0bits.U1RXIF = 0;
    LATDbits.LATD2 = 0;
    IEC0bits.U1RXIE = 1;
    IEC0bits.U1ARXIE = 1;
}


#pragma interrupt CN_ISR ipl5 vector 26
void CN_ISR (void) {
	
	IEC1bits.CNIE = 0;
    readD;
    UART_send();
    // int n;
    
    LATDbits.LATD1 = 1 - LATDbits.LATD1;
    // for (n = 0; n<3000; n++);
    
	IFS1bits.CNIF = 0;
	IEC1bits.CNIE = 1;
}
void CN_init(void){
	asm("di");
	CNCON = 0x8000;
	CNENbits.CNEN19 = 1; //CN19/RD13 as input
	CNPUEbits.CNPUE19 = 1;
    readD;
	IPC6SET = 0x00140000; //Set priority level = 5
	IPC6SET = 0x00030000; //Set subpriority level = 3
	IFS1CLR = 0x0001; //Clear interrupt flag
	IEC1SET = 0x0001;
	asm("ei");
}


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
    OSCCONbits.FRCDIV = 0x2; // the prescale of FRC is 8
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
    LATDbits.LATD2 = 1;
    IEC0SET = 0x0110;           // enable interrupt for timer 1 and timer 2
}

void UART_init(void) {
    asm("di");
    
    // The configuration will override PORTx config

    // Setup for UART Transmit
    U1MODEbits.BRGH = 0; // low speed
    U1BRG = 51; // baud rate: PBCLK / 16
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
}

void UART_send(void) {
    // start the transimission
    static int idx = 0;
    static char str_test[12] = "VE373_Lab_6";
    if (idx == 11){
        idx = 0;
        LCD_init();
        DelayMsec(40);
    }
    LATDbits.LATD0 = 1;
    while (U1STAbits.TRMT!=1) {
        ; //wait!!!if MCU is sending data
    }
    U1TXREG = str_test[idx];
    while (U1STAbits.TRMT!=1);
    LATDbits.LATD0 = 0;
    idx++;
}


int main() {
    MCU_init();
    UART_init();
    LCD_init();
    CN_init();
    
    LATDbits.LATD2 = 0;
    while (1) ;
    return 0;
}