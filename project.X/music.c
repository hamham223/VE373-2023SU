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
static int total_time = 31600;
static int notes[] = {C4, D4, E4, C4, G4, 0, 
                E4, 0, D4, G4, D4, 0, 
                C4, A3, E4, 0, C4, B3, 0,
                C4, B3, A3, 0, B3, 0, C4, D4, 0,
                G3, 0,  C4, 0, D4, E4, 0, F4, 0,
                F4, E4, 0, C4, D4, 0, 0};
static int durations[] = {3000, 3000, 3000, 3000, 6000, 3000, 
                  3000, 500, 5000, 5000, 2000, 3000, 
                  3000, 3000, 5000, 2000, 3000, 7000, 3000,
                  3000, 3000, 4000, 2000, 4000, 2000, 3000, 3000, 1500, 
                  4500, 2000, 4000, 2000, 3000, 3000, 2000, 6000, 3000,
                  3000, 5000, 2000, 2000, 6000, 10000, 10000};
static volatile int index = 0; // ?????
extern unsigned char global_index;
extern unsigned char global_count;

#define PWM_FREQUENCY_HZ  50   // ?? PWM ????

void configurePWM(void) {
    // ?? PWM ??
    T2CONbits.TON = 0;      // ?? Timer2
    T2CONbits.TCS = 0;      // ???????
    T2CONbits.TCKPS = 0b101; // ?????? 1:1
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

void configureT4(void) {
    // 
    global_index = 0;
    global_count = 0;
    T4CONbits.TON = 0;      // ??Timer4
    T4CONbits.TCS = 0;      // ???????
    T4CONbits.TCKPS = 0b111; // ????1:256
    PR4 = 1200;           // ????
    TMR4 = 0;               // ?????

    // ????
    IPC4bits.T4IP = 4;
    IFS0bits.T4IF = 0;     // ??Timer4?????
    IEC0bits.T4IE = 1;     // ??Timer4??

    T4CONbits.TON = 1;     // ??Timer4
}


// ???3??????
#pragma interrupt timer_4_interrupt ipl5 vector 16
void timer_4_interrupt(void) {
    TMR4 = 0;
    IFS0bits.T4IF = 0; 
    if (global_index/2 < sizeof(notes)){
        if (global_index%2 == 0){
            PR4 = durations[global_index/2]*total_time/8000;
            if (notes[global_index/2]==0){
                OC1RS = 0;
            }else{
                int halfPeriod = total_time / notes[global_index/2];
                PR2 = halfPeriod*2;
                OC1RS = halfPeriod;
            }    
        }
        else{
            PR4 = 50*total_time/8000;
            OC1RS = 0;
        }
        global_index +=1;
        if (global_index == 87) {
            global_index = 0;
            global_count += 1;
        }
    }
    else {
        OC1RS = 0;
    }
}

unsigned char getInitHeight(void) {
    return 1;
    // return 14 - durations[global_index]/1000;
}
