/*
 * Andy's Workshop Reflow Controller ATMega328p firmware
 * Copyright (c) 2015 Andy Brown. http://www.andybrown.me.uk
 * Please see website for licensing terms.
 */

#pragma once


namespace awreflow {


  /*
   * display a (physically) large number at a position on the display
   * maximum is 3 digits
   */

  class UiLargeNumberComponent {
    
    protected:
      uint8_t _x,_y;

    public:
      UiLargeNumberComponent(uint8_t x,uint8_t y);

      void setNumber(uint16_t number);
      void setSignedNumber(int16_t number);
  };


  /*
   * Constructor
   */

  inline UiLargeNumberComponent::UiLargeNumberComponent(uint8_t x,uint8_t y) 
    : _x(x),
      _y(y) {
  }


  /*
   * Set a new unsigned number
   */

  inline void UiLargeNumberComponent::setNumber(uint16_t number) {
    Nokia5110::writeLargeNumber(_x,_y,number);
  }


  /*
   * Set a new signed number
   */

  inline void UiLargeNumberComponent::setSignedNumber(int16_t number) {
    Nokia5110::writeLargeSignedNumber(_x,_y,number);
  }
}
