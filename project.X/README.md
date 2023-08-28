# Project: Music-Game on PIC32
> Author: :hamster: :one:

### Peripherals

+ JR12684-3A(吉润实业), UART, 3.3V
+ piezoresistor (pressure sensor)
+ buzzer

### Structures

+ `display.c`: **UART** init and libs for display on the screen

  ```C
  void SendData(const unsigned char dat){
  	while (U1STAbits.TRMT!=1);
  	U1TXREG = dat;
  	while (U1STAbits.TRMT!=1);
  }
  ```

  The basic libs include drawing a line, a rectangle, showing some numbers and so on

+ `gameplay.c` : game logic implementation, including *combos*, *scores* and *hits*

+ `measure.c`: get pressure from the sensor by **ADC** module

+ `measure.c`: control the looped music using **Timer** and **PWM**
