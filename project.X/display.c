#include <p32xxxx.h>
#include "display.h"

static const unsigned char COMMAND_BEGIN[2] = {0xfe, 0xfd};
static const unsigned char COMMAND_END[4] = {0xdd, 0xcc, 0xbb, 0xaa};

void UART_init(void) {
    asm("di");
    
    // The configuration will override PORTx config

    // Setup for UART Transmit
    U1MODEbits.BRGH = 1; // low speed
    U1BRG = 8; // baud rate: PBCLK / 4 / 9
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