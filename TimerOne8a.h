/*
 *  Optimized Interrupt and PWM utilities for 16-bit Timer1 on ATmega8A
 *  Based on original code by Jesse Tane and Paul Stoffregen
 *  Modified and optimized for ATmega8A by OmarTazul
 *  March 03, 2025
 *
 *  Licensed under Creative Commons Attribution 3.0 United States License.
 */

 #ifndef TIMERONE8A_H_
 #define TIMERONE8A_H_
 
 #include <Arduino.h>        // Include Arduino core functions
 #include <avr/interrupt.h>
 
 #define TIMER1_A_PIN     9
 #define TIMER1_B_PIN     10
 #define TIMER1_ICP_PIN   8
 #define TIMER1_CLK_PIN   5
 
 #define TIMER1_RESOLUTION 65536UL  // Timer1 is 16-bit
 
 class TimerOne
 {
 public:
	 void initialize(unsigned long microseconds = 1000000UL)
	 {
		 TCCR1A = 0;
		 TCCR1B = _BV(WGM13);  // Phase and Frequency Correct PWM
		 setPeriod(microseconds);
	 }
 
	 void setPeriod(unsigned long microseconds)
	 {
		 unsigned long cycles = (F_CPU / 2000000UL) * microseconds;
 
		 uint8_t prescalerBits;
		 if (cycles < TIMER1_RESOLUTION)
		 {
			 prescalerBits = _BV(CS10);          // No prescaling
		 }
		 else if ((cycles >>= 3) < TIMER1_RESOLUTION)
		 {
			 prescalerBits = _BV(CS11);          // Prescale by 8
		 }
		 else if ((cycles >>= 3) < TIMER1_RESOLUTION)
		 {
			 prescalerBits = _BV(CS11) | _BV(CS10); // Prescale by 64
		 }
		 else if ((cycles >>= 2) < TIMER1_RESOLUTION)
		 {
			 prescalerBits = _BV(CS12);          // Prescale by 256
		 }
		 else if ((cycles >>= 2) < TIMER1_RESOLUTION)
		 {
			 prescalerBits = _BV(CS12) | _BV(CS10); // Prescale by 1024
		 }
		 else
		 {
			 cycles = TIMER1_RESOLUTION -1;
			 prescalerBits = _BV(CS12) | _BV(CS10); // Prescale by 1024
		 }
 
		 ICR1 = (uint16_t)cycles;
		 TCCR1B = _BV(WGM13) | prescalerBits;
 
		 clockSelectBits = prescalerBits;
		 pwmPeriod = cycles;
	 }
 
	 void start()
	 {
		 TCCR1B = 0;
		 TIFR |= _BV(TOV1);  // Clear overflow flag
		 TCNT1 = 0;
		 resume();
	 }
 
	 void stop()
	 {
		 TCCR1B = _BV(WGM13);
	 }
 
	 void restart()
	 {
		 start();
	 }
 
	 void resume()
	 {
		 TCCR1B = _BV(WGM13) | clockSelectBits;
	 }
 
	 void pwm(uint8_t pin, uint16_t duty, unsigned long microseconds = 0)
	 {
		 if (microseconds > 0) setPeriod(microseconds);
		 if (pin == TIMER1_A_PIN)
		 {
			 pinMode(TIMER1_A_PIN, OUTPUT);
			 TCCR1A |= _BV(COM1A1);
		 }
		 else if (pin == TIMER1_B_PIN)
		 {
			 pinMode(TIMER1_B_PIN, OUTPUT);
			 TCCR1A |= _BV(COM1B1);
		 }
 
		 setPwmDuty(pin, duty);
		 TCCR1B = _BV(WGM13) | clockSelectBits;
	 }
 
	 void setPwmDuty(uint8_t pin, uint16_t duty)
	 {
		 uint16_t dutyCycle = (pwmPeriod * duty) / 1023;
		 if (pin == TIMER1_A_PIN)
			 OCR1A = dutyCycle;
		 else if (pin == TIMER1_B_PIN)
			 OCR1B = dutyCycle;
	 }
 
	 void disablePwm(uint8_t pin)
	 {
		 if (pin == TIMER1_A_PIN)
			 TCCR1A &= ~_BV(COM1A1);
		 else if (pin == TIMER1_B_PIN)
			 TCCR1A &= ~_BV(COM1B1);
	 }
 
	 void attachInterrupt(void (*isr)(), unsigned long microseconds = 0)
	 {
		 if (microseconds > 0) setPeriod(microseconds);
		 isrCallback = isr;
		 TIMSK |= _BV(TOIE1);
	 }
 
	 void detachInterrupt()
	 {
		 TIMSK &= ~_BV(TOIE1);
	 }
 
	 static void (*isrCallback)();
 
 private:
	 static unsigned long pwmPeriod;
	 static uint8_t clockSelectBits;
 };
 
 extern TimerOne Timer1;
 
 #endif
 