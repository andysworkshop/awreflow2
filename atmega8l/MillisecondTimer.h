/*
 * Andy's Workshop Reflow Controller ATMega328p firmware
 * Copyright (c) 2015 Andy Brown. http://www.andybrown.me.uk
 * Please see website for licensing terms.
 */

#pragma once


namespace awreflow {

  /*
   * Use Timer0 to count in milliseconds
   */

  class MillisecondTimer {

    public:
      static volatile uint32_t _counter;
      static volatile uint8_t _subCounter;

    public:
      static void setup();
      static uint32_t millis();
      static void delay(uint32_t waitfor);

      static uint32_t difference(uint32_t start);
      static bool hasTimedOut(uint32_t start,uint32_t timeout);
  };


  /*
   * Setup timer 0. It will tick at 1MHz (CLK/8) and count 0-255 then generate an overflow
   * interrupt. So interrupt frequency = (CLK/8)*256 = 256uS. We will increment the millisecond
   * timer every 4 ticks = 1.024ms. Each interrupt we add 6 to the counter register which has the
   * effect of changing the interrupt frequency to (CLK/8)*250 and that gives us an accurate 1ms counter. 
   */

  inline void MillisecondTimer::setup() {

    // set up timer 0

    TCCR0 |= (1 << CS01);     // CLK/8
    TIMSK |= (1 << TOIE0);    // Timer0 overflow interrupt
  }


  /*
   * Return the timer
   */

  inline uint32_t MillisecondTimer::millis() {
    
    uint32_t ms;

    // an 8-bit MCU cannot atomically read/write a 32-bit value so we must
    // disable interrupts while retrieving the value to avoid getting a half-written
    // value if an interrupt gets in while we're reading it

    cli();
    ms=_counter;
    sei();

    return ms;
  }


  /*
   * Simple delay method
   */

  inline void MillisecondTimer::delay(uint32_t waitfor) {

    uint32_t target;

    target=millis()+waitfor;
    while(_counter<target);
  }


  /*
   * Return the difference between a starting time and now, taking into account
   * wrap around
   */

  inline uint32_t MillisecondTimer::difference(uint32_t start) {

    uint32_t now=millis();

    if(now>start)
      return now-start;
    else
      return now+(0xffffffff-start+1);
  }  


  /*
   * Check if a timeout has been exceeded. This is designed to cope with wrap around
   */

  inline bool MillisecondTimer::hasTimedOut(uint32_t start,uint32_t timeout) {
    return difference(start)>timeout;
  }
}
