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

#ifndef TimerOne8a_h_
#define TimerOne8a_h_

#include "Arduino.h"

#define TIMER1_A_PIN 9
#define TIMER1_B_PIN 10
#define TIMER1_ICP_PIN 8
#define TIMER1_CLK_PIN 5

#define TIMER1_RESOLUTION 65536UL // Timer1 is 16 bit

class TimerOne
{
public:
	//****************************
	//  Configuration
	//****************************
	void initialize(unsigned long microseconds = 1000000) __attribute__((always_inline))
	{
		TCCR1B = _BV(WGM13); // set mode as phase and frequency correct pwm, stop the timer
		TCCR1A = 0;			 // clear control register A
		setPeriod(microseconds);
	}
	void setPeriod(unsigned long microseconds) __attribute__((always_inline))
	{
		const unsigned long cycles = ((F_CPU / 100000 * microseconds) / 20);
		if (cycles < TIMER1_RESOLUTION)
		{
			clockSelectBits = _BV(CS10);
			pwmPeriod = cycles;
		}
		else if (cycles < TIMER1_RESOLUTION * 8)
		{
			clockSelectBits = _BV(CS11);
			pwmPeriod = cycles / 8;
		}
		else if (cycles < TIMER1_RESOLUTION * 64)
		{
			clockSelectBits = _BV(CS11) | _BV(CS10);
			pwmPeriod = cycles / 64;
		}
		else if (cycles < TIMER1_RESOLUTION * 256)
		{
			clockSelectBits = _BV(CS12);
			pwmPeriod = cycles / 256;
		}
		else if (cycles < TIMER1_RESOLUTION * 1024)
		{
			clockSelectBits = _BV(CS12) | _BV(CS10);
			pwmPeriod = cycles / 1024;
		}
		else
		{
			clockSelectBits = _BV(CS12) | _BV(CS10);
			pwmPeriod = TIMER1_RESOLUTION - 1;
		}
		ICR1 = pwmPeriod;
		TCCR1B = _BV(WGM13) | clockSelectBits;
	}

	//****************************
	//  Run Control
	//****************************
	void start() __attribute__((always_inline))
	{
		TCCR1B = 0;
		TCNT1 = 0; // TODO: does this cause an undesired interrupt?
		resume();
	}
	void stop() __attribute__((always_inline))
	{
		TCCR1B = _BV(WGM13);
	}
	void restart() __attribute__((always_inline))
	{
		start();
	}
	void resume() __attribute__((always_inline))
	{
		TCCR1B = _BV(WGM13) | clockSelectBits;
	}

	//****************************
	//  PWM outputs
	//****************************
	void setPwmDuty(char pin, unsigned int duty) __attribute__((always_inline))
	{
		unsigned long dutyCycle = pwmPeriod;
		dutyCycle *= duty;
		dutyCycle >>= 10;
		if (pin == TIMER1_A_PIN)
			OCR1A = dutyCycle;
#ifdef TIMER1_B_PIN
		else if (pin == TIMER1_B_PIN)
			OCR1B = dutyCycle;
#endif
#ifdef TIMER1_C_PIN
		else if (pin == TIMER1_C_PIN)
			OCR1C = dutyCycle;
#endif
	}
	void pwm(char pin, unsigned int duty) __attribute__((always_inline))
	{
		if (pin == TIMER1_A_PIN)
		{
			pinMode(TIMER1_A_PIN, OUTPUT);
			TCCR1A |= _BV(COM1A1);
		}
#ifdef TIMER1_B_PIN
		else if (pin == TIMER1_B_PIN)
		{
			pinMode(TIMER1_B_PIN, OUTPUT);
			TCCR1A |= _BV(COM1B1);
		}
#endif
#ifdef TIMER1_C_PIN
		else if (pin == TIMER1_C_PIN)
		{
			pinMode(TIMER1_C_PIN, OUTPUT);
			TCCR1A |= _BV(COM1C1);
		}
#endif
		setPwmDuty(pin, duty);
		TCCR1B = _BV(WGM13) | clockSelectBits;
	}
	void pwm(char pin, unsigned int duty, unsigned long microseconds) __attribute__((always_inline))
	{
		if (microseconds > 0)
			setPeriod(microseconds);
		pwm(pin, duty);
	}
	void disablePwm(char pin) __attribute__((always_inline))
	{
		if (pin == TIMER1_A_PIN)
			TCCR1A &= ~_BV(COM1A1);
#ifdef TIMER1_B_PIN
		else if (pin == TIMER1_B_PIN)
			TCCR1A &= ~_BV(COM1B1);
#endif
#ifdef TIMER1_C_PIN
		else if (pin == TIMER1_C_PIN)
			TCCR1A &= ~_BV(COM1C1);
#endif
	}

	//****************************
	//  Interrupt Function
	//****************************

	void attachInterrupt(void (*isr)()) __attribute__((always_inline))
	{
		isrCallback = isr;
		TIMSK = _BV(TOIE1);
	}
	void attachInterrupt(void (*isr)(), unsigned long microseconds) __attribute__((always_inline))
	{
		if (microseconds > 0)
			setPeriod(microseconds);
		attachInterrupt(isr);
	}
	void detachInterrupt() __attribute__((always_inline))
	{
		TIMSK = 0;
	}
	static void (*isrCallback)();
	static void isrDefaultUnused();

private:
	// properties
	static unsigned short pwmPeriod;
	static unsigned char clockSelectBits;
};

extern TimerOne Timer1;

#endif