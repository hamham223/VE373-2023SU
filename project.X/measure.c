#include <p32xxxx.h>
#include "measure.h"

void ADC_init(void){
    AD1PCFG = 0xFFF7; // PORTB = Digital; RB3 = analog
	AD1CON1 = 0x40;// Timer3 period match ends sampling, integer 16-bit
	AD1CHS = 0x00030000;// Connect RB3/AN3 as CH0 input

	AD1CSSL = 0;
	AD1CON3 = 0x0000;// Sample time is TMR3, TAD = internal TPB * 2
	AD1CON2 = 0x0000;// Interrupt at the end of each conversion

    T3CONSET = 0x0;
    PR3 = 8000;
    T3CONbits.TCKPS = 0b11;
    T3CONSET = 0x8000;
	AD1CON1SET = 0x0004;// start auto sampling every 2 mSecs
    AD1CON1SET = 0x8000; // turn ON the ADC
}

double getPressure(void) {
    volatile double voltage = 0;
		int i = 0;
        voltage = 0;
        for (; i < 40 ; i++)
		{
			while (!IFS1 & 0x0002){};// conversion done?
			voltage += (double)ADC1BUF0;
			IFS1CLR = 0x0002;// clear ADIF
		}
        voltage=voltage/1024.0/40*3.3; // display the average

    return voltage;
}

void SendData2(const unsigned char dat){
	while (U5STAbits.TRMT!=1);
	U5TXREG = dat;
	while (U5STAbits.TRMT!=1);
}

void SendString2(const unsigned char *s, unsigned int size){
	LATDbits.LATD1 = 1;
	unsigned int i;
	for(i = 0; i < size; i++){
		U5TXREG = *s;
		while (U5STAbits.TRMT!=1);
		s++;
		delay(20);
	}
	LATDbits.LATD1 = 0;
}

void UART2_init(void) {
    asm("di");
    
    // Setup for UART3 Transmit
	U5MODEbits.BRGH = 1; 
	U5BRG = 16;
    // The most common data format is 8,N,1 

	U5MODEbits.PDSEL = 0b00;
	U5MODEbits.STSEL = 0;
	// U5MODEbits.UARTEN = 0;
	
    // no inter-transmit interrupted are needed, skip
	U5STAbits.UTXEN = 1;
	IFS2bits.U5TXIF = 0; // IF clear
	
    // config the UART Recieve
    // baud rate, stop bit are already configed;

	U5MODEbits.ON = 0;
	U5MODEbits.ON = 1;

    asm("ei");
    delay(100);
}