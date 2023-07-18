#include <p32xxxx.h>
#include "music.h"

// Define notes here
#define G3 98
#define A3 110
#define B3 123
#define C4 131
#define D4 147
#define E4 165
#define F4 175
#define G4 196
#define A4 220
#define B4 247

// int total_time = 19575;
static int total_time = 24000;
static int notes[] = {C4, D4, E4, C4, G4, 0, 
                E4, D4, G4, D4, 0, 
                C4, A3, E4, C4, B3, 0,
                C4, B3, A3, B3, C4, D4, 0,
                 G3, C4, D4, E4, F4, 0,
                F4, E4, D4, D4, 0};
static int durations[] = {100, 100, 100, 100, 300, 100, 
                  100, 100, 300, 120, 80, 
                  100, 100, 200, 100, 300,100,
                  100, 100, 150, 150, 200,100, 100, 
                  100, 100, 100, 100, 300,100,
                  120, 100, 200, 300, 100000};
static volatile int index = 0; // ?????


#define PWM_FREQUENCY_HZ  50   // ?? PWM ????

void configurePWM(void) {
    // ?? PWM ??
    T2CONbits.TON = 0;      // ?? Timer2
    T2CONbits.TCS = 0;      // ???????
    T2CONbits.TCKPS = 0b00; // ?????? 1:1
    TMR2 = 0;               // ?????
    PR2 = 0; // ????

    OC1CONbits.ON = 0;      // ?? OC1 ??
    OC1CONbits.OCM = 0b110; // PWM ?????? OC1RS ? OC1R ??
    OC1CONbits.OCTSEL = 0;  // ?? Timer2 ?????
    OC1R = 0;               // ?? OCR ???????????? 0
    OC1RS = 0;              // ????????????????? 0
    OC1CONbits.ON = 1;      // ?? OC1 ??

    T2CONbits.TON = 1;      // ?? Timer2
}

void configureT3(void) {
    // 
    T3CONbits.TON = 0;      // ??Timer3
    T3CONbits.TCS = 0;      // ???????
    T3CONbits.TCKPS = 0b111; // ????1:256
    PR3 = 100;           // ????
    TMR3 = 0;               // ?????

    // ????
    IPC3bits.T3IP = 4;     // ????????4
    IFS0bits.T3IF = 0;     // ??Timer3?????
    IEC0bits.T3IE = 1;     // ??Timer3??

    T3CONbits.TON = 1;     // ??Timer3
}


// ???3??????
#pragma interrupt timer_3_interrupt ipl4 vector 12
void timer_3_interrupt(void) {
    TMR3 = 0;               // ?????
    IFS0bits.T3IF = 0; // ?? Timer3 ?????
    if (index/2<sizeof(notes)){
        if (index%2==0){
            PR3 = durations[index/2]*total_time/8000;
            if (notes[index/2]==0){
                OC1RS = 0;
            }else{
                int halfPeriod = total_time / notes[index/2];
                PR2=halfPeriod*2;
                OC1RS = halfPeriod;
            }    
        }
        else{
            PR3 = 50*total_time/8000;
            OC1RS = 0;
        }
        index+=1;
    }
    else {
        OC1RS = 0;
    }
}