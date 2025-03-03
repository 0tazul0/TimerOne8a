/*
 *  Optimized Interrupt and PWM utilities for 16-bit Timer1 on ATmega8A
 *  Based on original code by Jesse Tane and Paul Stoffregen
 *  Modified and optimized for ATmega8A by OmarTazul
 *  March 03, 2025
 *
 *  Licensed under Creative Commons Attribution 3.0 United States License.
 */

 #include "TimerOne8a.h"
 #include <avr/interrupt.h>
 
 TimerOne Timer1;
 
 unsigned long TimerOne::pwmPeriod = 0;
 uint8_t TimerOne::clockSelectBits = 0;
 void (*TimerOne::isrCallback)() = 0;
 
 ISR(TIMER1_OVF_vect)
 {
     if (Timer1.isrCallback)
         Timer1.isrCallback();
 }
 