/*
 * Andy's Workshop Reflow Controller ATMega328p firmware
 * Copyright (c) 2015 Andy Brown. http://www.andybrown.me.uk
 * Please see website for licensing terms.
 */


#pragma once


namespace awreflow {


  /*
   * Debounced action button handler
   */

  class ActionButton {

    protected:
      uint16_t _state;              // shift register for sampling the state
      bool _pressed;                // set when pressed and latched until collected
      uint32_t _lastReadTime;       // last sample time. samples are made at most once per ms.
      uint32_t _pressTime;          // time of the last press

      enum {
        PRESS_TIMEOUT = 250         // after this many ms a press isn't valid any more (better than lag)
      };

    public:
      ActionButton();

      bool isPressed();
      void loop();
  };


  /*
   * Constructor 
   */

  inline ActionButton::ActionButton() {
    _state=0xffff;
    _lastReadTime=0;
    _pressed=false;
  }


  /*
   * loop
   */

  inline void ActionButton::loop() {

    uint32_t now;
    bool stateNow;

    stateNow=GpioActionButton::read();  
    now=MillisecondTimer::millis();

    // sample at most once per ms

    if(now!=_lastReadTime) {
      
      // shift method based on http://www.ganssle.com/debouncing-pt2.htm

      _lastReadTime=now;
      _state=(_state << 1) | 0xe000 | stateNow;

      if(_state==0xf000 && !_pressed) {
        _pressTime=MillisecondTimer::millis();
        _pressed=true;
      }
    }
  }


  /*
   * Check if pressed since last sample. It's pressed if there's been
   * an up->down transition
   */

  inline bool ActionButton::isPressed() {

    if(_pressed) {

      _pressed=false;

      // button was pressed, was it within our tolerance?

      return !MillisecondTimer::hasTimedOut(_pressTime,PRESS_TIMEOUT);
    }

    return false;
  }
}
