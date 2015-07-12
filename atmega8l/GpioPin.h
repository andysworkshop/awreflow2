/*
 * Andy's Workshop Reflow Controller ATMega328p firmware
 * Copyright (c) 2015 Andy Brown. http://www.andybrown.me.uk
 * Please see website for licensing terms.
 */

#pragma once


namespace awreflow {

  /*
   * GPIO ports
   */

  struct GPIOB {
    enum {
      Port = 0x18,
      Dir  = 0x17,
      Pin  = 0x16
    };
  };

  struct GPIOC {
    enum {
      Port = 0x15,
      Dir  = 0x14,
      Pin  = 0x13
    };
  };

  struct GPIOD {
    enum {
      Port = 0x12,
      Dir  = 0x11,
      Pin  = 0x10
    };
  };


  /*
   * Base template for all GPIO pins. Provide support for set/reset
   */

  template<uint8_t TPort,uint8_t TPin>
  struct GpioPin {
   
    /*
     * Immediately set a pin to HIGH
     */

    static void set() {
      asm volatile(
        "  sbi  %[port],%[pin]  \n\t"
        :: [port] "I" (TPort),
           [pin]  "I" (TPin)
      );
    }


    /*
     * Immediately set a pin to LOW
     */

    static void reset() {
      asm volatile(
        "  cbi  %[port],%[pin]  \n\t"
        :: [port] "I" (TPort),
           [pin]  "I" (TPin)
      );
    }


    /*
     * Set a pin to HIGH with 'wait' nop instructions beforehand. Used to ensure that an
     * active low CS pin waits for its setup delay time before activating. nop's cannot
     * reliably be inserted in a separate asm block to the actual action associated with the delay.
     */

    static void setWithPreWait(uint8_t wait) {

      asm volatile(
        "noploop%=:              \n\t"
        "  nop                   \n\t"
        "  dec   %[nopcount]     \n\t"
        "  brne  noploop%=       \n\t"
        "  sbi   %[port],%[pin]  \n\t"
        :: [port]     "I" (TPort),
           [pin]      "I" (TPin),
           [nopcount] "d" (wait)
      );
    }


    /*
     * Set a pin to LOW with 'wait' nop instructions afterwards. Used to ensure that an
     * active low CS pin satisifies its setup time before clocking starts. nop's cannot
     * reliably be inserted in a separate asm block to the actual action associated with the delay.
     */

    static void resetWithPostWait(uint8_t wait) {

      asm volatile(
        "  cbi   %[port],%[pin]  \n\t"
        "noploop%=:              \n\t"
        "  nop                   \n\t"
        "  dec   %[nopcount]     \n\t"
        "  brne  noploop%=       \n\t"
        :: [port]     "I" (TPort),
           [pin]      "I" (TPin),
           [nopcount] "d" (wait)
      );
    }


    /*
     * Change state to the given state immediately
     */

    static void changeTo(bool state) {
      if(state)
        set();
      else
        reset();
    }
  };


  /*
   * GPIO output, provide support for init
   */

  template<class TPort,uint8_t TPin>
  struct GpioOutputPin : GpioPin<TPort::Port,TPin> {

    /*
     * Setup the port pin by setting the direction bit in the port register
     */

    static void setup() {    
      asm volatile(
        "  sbi  %[port],%[pin]  \n\t"
        :: [port] "I" (TPort::Dir),
           [pin]  "I" (TPin)
      );
    }
  };


  /*
   * GPIO input, provide support for init, read
   */

  template<class TPort,uint8_t TPin>
  struct GpioInputPin : GpioPin<TPort::Port,TPin> {

    /*
     * Setup the pin by clearing the direction bit in the port register
     */

    static void setup() {    
      asm volatile(
        "  cbi  %[port],%[pin]  \n\t"
        :: [port] "I" (TPort::Dir),
           [pin]  "I" (TPin)
      );
    }


    /*
     * Read the current value of an input pin
     */

    static bool read() {
      
      uint8_t r;

      asm volatile(
          "  clr  %[result]       \n\t"       // result = 0
          "  sbic %[port],%[pin]  \n\t"       // skip next if port bit is clear
          "  inc  %[result]       \n\t"       // result = 1
        : [result] "=r" (r)
        : [port]   "I"  (TPort::Pin),
          [pin]    "I"  (TPin)
      );

      return r;
    }
  };


  /*
   * All pins used in this project
   */

  typedef GpioOutputPin<GPIOD,1> GpioUartTx;
  typedef GpioInputPin<GPIOD,0> GpioUartRx;

  typedef GpioOutputPin<GPIOC,2> GpioScreenDataCommand;
  typedef GpioOutputPin<GPIOC,0> GpioScreenSelect;
  typedef GpioOutputPin<GPIOC,1> GpioScreenReset;

  struct GpioScreenLed : GpioOutputPin<GPIOB,1> {

    static void setup() {

      // call the base class to set as output

      GpioOutputPin<GPIOB,1>::setup();

      // initialise timer1. clock = 8MHz/8 = 1MHz
      // PWM = 10 bit (1024 counts). PWM freq = 125kHz/512 = 244Hz

      OCR1A = 0;                                         // clear the duty cycle counter
      TCCR1A = (1 << COM1A1) | (1 << WGM11) | (1 << WGM10);  // non-inverting 10 bit phase correct
      TCCR1B = (1 << CS11);                                  // CLK/8
    }
  };

  typedef GpioInputPin<GPIOD,7> GpioEncoderA;
  typedef GpioInputPin<GPIOD,6> GpioEncoderB;
  typedef GpioInputPin<GPIOB,0> GpioActionButton;

  typedef GpioOutputPin<GPIOD,3> GpioActivateOven;

  typedef GpioOutputPin<GPIOD,5> GpioPairedStatus;

  typedef GpioOutputPin<GPIOC,3> GpioDebugOut;


  /*
   * Zero-sense needs pin change interrupt
   */

  struct GpioZeroSense : GpioInputPin<GPIOD,2> {
    
    static void setup() {
      
      // set as input. PD2 = INT0

      GpioInputPin<GPIOD,2>::setup();

      // configure the INT0 interrupt

      MCUCR = (1 << ISC01);    // falling edge of INT0
      GICR = (1 << INT0);      // INT0 enabled
    }
  };

  typedef GpioOutputPin<GPIOB,2> GpioSpiCs;
  typedef GpioOutputPin<GPIOB,3> GpioSpiMosi;
  typedef GpioInputPin<GPIOB,4> GpioSpiMiso;
  typedef GpioOutputPin<GPIOB,5> GpioSpiClk;
}

