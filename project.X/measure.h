#ifndef _MEASURE_H
#define _MEASURE_H

static const double pressThreshold = 20000000;

double getPressure(void);
void ADC_init(void);

void UART2_init(void);
void SendData2(const unsigned char dat);
void SendString2(const unsigned char *s, unsigned int size);

// TODO: Add more function if necessary

#endif /* meaure.h */