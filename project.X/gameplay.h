#ifndef GAMEPLAY_H
#define	GAMEPLAY_H

#include "display.h"

uchar combo;

uchar Check_Remove_Rect(uchar rectx,uchar recty,uchar x,uchar y);
pt checkHits(pt a,uchar x,uchar y,double volt_gate_l,double volt_gate_r,double volt);

void showInterface(void);
void comboClear(void);

#endif	/* GAMEPLAY_H */

