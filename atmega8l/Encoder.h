/*
 * Andy's Workshop Reflow Controller ATMega328p firmware
 * Copyright (c) 2015 Andy Brown. http://www.andybrown.me.uk
 * Please see website for licensing terms.
 */


#pragma once


namespace awreflow {


  /*
   * Rotary encoder management. Call loop() frequently to check for changes. A change is stored until
   * you call getChange() to retrieve it. Only then will subsequent changes start to be registered.
   */

  class Encoder {

    protected:
      bool _lastA;
      int8_t _lastChange;     // -1, 0, 1

    public:
      Encoder();

      void loop();

      int8_t getChange();      
  };


  /*
   * Constructor
   */

  inline Encoder::Encoder() {
    
    _lastA=false;
    _lastChange=0;
  }


  /*
   * Loop: check for change but only store it if we've been and picked up the previous change
   */

  inline void Encoder::loop() {

    bool newA;

    newA=GpioEncoderA::read();

    if(_lastChange==0) {
      if(!_lastA && newA) {
        if(GpioEncoderB::read())
          _lastChange=1;
        else
          _lastChange=-1;
      }
    }

    _lastA=newA;
  }


  /*
   * Get the change since the last call here. After this call the next change will be registered
   */

  inline int8_t Encoder::getChange() {

    int8_t ret;

    ret=_lastChange;
    _lastChange=0;
   
    return ret;
  }
}
