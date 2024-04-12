/*
 *  Interrupt and PWM utilities for 16 bit Timer1 on ATmega8a
 *	Stripdown Version from Paul Stoffregen TimerOne Library
 *	Original code by Jesse Tane for http://labs.ideo.com August 2008
 *  Modified 13 April 2024 by OmarTazul Only for Atmega8a
 *
 *  This is free software. You can redistribute it and/or modify it under
 *  the terms of Creative Commons Attribution 3.0 United States License.
 *  To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/us/
 *  or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 *
 */

#include "TimerOne8a.h"

TimerOne Timer1;              // preinstatiate

unsigned short TimerOne::pwmPeriod = 0;
unsigned char TimerOne::clockSelectBits = 0;
void (*TimerOne::isrCallback)() =  NULL;

// interrupt service routine that wraps a user defined function supplied by attachInterrupt

ISR(TIMER1_OVF_vect)
{
  Timer1.isrCallback();
}