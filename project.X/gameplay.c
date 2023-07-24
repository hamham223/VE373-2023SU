#include "gameplay.h"
#include "measure.h"
#include "display.h"
#include "music.h"
#include <p32xxxx.h>

uchar Check_Remove_Rect(uchar rectx,uchar recty,uchar x,uchar y){
    uchar flag=0;
    if(rectx<x&&rectx+BLOCK_WIDTH>x){
        if(recty<y&&recty+BLOCK_HEIGHTH>y){
            //count+1
            flag=1;
            clearRectangle(rectx,recty, rectx+BLOCK_WIDTH,recty+BLOCK_HEIGHTH, 0);
            delay(5);
        }
    }
    return flag;
}


/*void Volt_to_track(){
    double the_volt=0;
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
*/

static int hit = 0;
extern uchar combo;
extern uchar global_index;
const char status[15] = "HHHHGGGGPPPP";

static void showHits(uchar st){

    if (hit==10) {
        // a new hit
        showChar(106, 9, status[st]);
    }
    else if (hit==0) {
        // clear
        showChar(106, 9, ' ');
    }
    hit--;
}

static void updateScore(void) {
    static uchar score = 0;
    score += 1;
    showNumber(104, 45, score);
    combo += 1;
    showNumber(104, 27, combo);
    delay(1);
}

void comboClear(void) {
    if (combo > 0) showNumber(104, 27, combo);
    combo = 0;
}

pt checkHits(pt a,uchar x,uchar y,double volt_gate_l,double volt_gate_r,double volt){
    if (a.t==0) return a;
    pt result;
    result.x=a.x;
    result.y=a.y;
    result.t=a.t;
    uchar status = 0;
    
    if(a.x<x&&a.x+BLOCK_WIDTH>x){
        if(a.y<y&&a.y+BLOCK_HEIGHTH>y){
            if (volt<volt_gate_r&&volt>volt_gate_l){
                result.t=0;
                hit = 10;
                status = y - a.y;
                clearRectangle(a.x,a.y, a.x+BLOCK_WIDTH,a.y+BLOCK_HEIGHTH, 0);
                updateScore();
            }
        }
    }
    if (hit >= 0) showHits(status);
    delay(2);
    return result;
}

void showInterface(void) {
    drawLine(90, 0, 90, 63, 0, 0);
    drawLine(22, 0, 22, 63, 1, 2);
    drawLine(45, 0, 45, 63, 1, 2);
    drawLine(67, 0, 67, 63, 1, 2);
    
    drawRectangle(0x00, 0x00, 0x7f, 0x3f, 0);
    setColorBoard(0);
    drawLine(6, 63, 6 + BLOCK_WIDTH, 63, 0, 0);
    drawLine(28, 63, 28 + BLOCK_WIDTH, 63, 0, 0);
    drawLine(51, 63, 51 + BLOCK_WIDTH, 63, 0, 0);
    drawLine(73, 63, 73 + BLOCK_WIDTH, 63, 0, 0);
    drawLine(6, 0, 6 + BLOCK_WIDTH, 0, 0, 0);
    drawLine(28, 0, 28 + BLOCK_WIDTH, 0, 0, 0);
    drawLine(51, 0, 51 + BLOCK_WIDTH, 0, 0, 0);
    drawLine(73, 0, 73 + BLOCK_WIDTH, 0, 0, 0);
    setColorBoard(1);

    drawCross(22, 52);
    drawCross(45, 52);
    drawCross(67, 52);

    drawRectangle(98, 6, 118, 21, 0);
    //showChar(106, 9, 'H');
    drawRectangle(98, 24, 118, 39, 0);
    showChar(104, 27, '0');
    showChar(110, 27, '0');
    drawRectangle(98, 42, 118, 57, 0);
    showChar(104, 45, '0');
    showChar(110, 45, '0');
}


void CN_init(void){
	asm("di");
	CNCON = 0x8000;
	CNENbits.CNEN19 = 1; //CN19/RD13 as input
	CNPUEbits.CNPUE19 = 1;
	PORTDbits.RD13;
	IPC6SET = 0x00140000; //Set priority level = 5
	IPC6SET = 0x00030000; //Set subpriority level = 3
	IFS1CLR = 0x0001; //Clear interrupt flag
	IEC1SET = 0x0001;
	asm("ei");
}

