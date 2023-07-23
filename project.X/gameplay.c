#include "gameplay.h"
#include "measure.h"
#include "display.h"
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

static void showHits(void){
    if (hit==10) showChar(106, 9, 'H');
    else if (hit==0) showChar(106, 9, ' ');
    hit--;
}

pt new_Check_Remove_Rect(pt a,uchar x,uchar y,double volt_gate_l,double volt_gate_r,double volt){
    pt result;
    result.x=a.x;
    result.y=a.y;
    result.t=a.t;
    if (result.t==0) return result ;
    
    if(a.x<x&&a.x+BLOCK_WIDTH>x){
        if(a.y<y&&a.y+BLOCK_HEIGHTH>y){
            
            if (volt<volt_gate_r&&volt>volt_gate_l){
                result.t=0;
                hit = 10;
                clearRectangle(a.x,a.y, a.x+BLOCK_WIDTH,a.y+BLOCK_HEIGHTH, 0);
            }
            
        }
    }
    if (hit >= 0) showHits();
    delay(2);
    return result;
}
