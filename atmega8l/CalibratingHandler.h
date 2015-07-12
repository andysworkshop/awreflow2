/*
 * Andy's Workshop Reflow Controller ATMega328p firmware
 * Copyright (c) 2015 Andy Brown. http://www.andybrown.me.uk
 * Please see website for licensing terms.
 */

#pragma once


namespace awreflow {


  /*
   * Handler for the zero percentage calibration. Encoder +/- is used to select the
   * new value and store it in the EEPROM.
   */

  class CalibratingHandler {

    public:

    protected:
      Encoder& _encoder;
      ActionButton& _actionButton;
      OvenControl& _oven;

      UiCalibratingDisplay _display;

      uint8_t _value;
      uint32_t _lastActionTime;

      enum {
        INACTION_TIMEOUT = 30000
      };

    public:
      CalibratingHandler(Encoder& encoder,ActionButton& button,OvenControl& oven);

      ProgramState loop();
  };


  /*
   * Constructor
   */

  inline CalibratingHandler::CalibratingHandler(Encoder& encoder,ActionButton& actionButton,OvenControl& oven) 
    : _encoder(encoder),
      _actionButton(actionButton),
      _oven(oven) {

    // read the value from EEPROM

    _value=Eeprom::Reader::zeroPercentage();
    _display.setNumber(_value);

    // switch the oven to the current value

    _oven.setDutyCycle(_value,true);

    // set the last action timeout to now

    _lastActionTime=MillisecondTimer::millis();
  }


  /*
   * Main loop
   */

  inline ProgramState CalibratingHandler::loop() {

    int8_t movement;

    do {

      // main loops

      _encoder.loop();
      _actionButton.loop();

      // check for encoder change

      if((movement=_encoder.getChange())!=0) {

        if(movement<0 && -movement>static_cast<int16_t>(_value))
          _value=0;           // decrement would go below zero 
        else
          _value=min<uint8_t>(100,_value+movement);

        // set the new value to the display and the oven 

        _display.setNumber(_value);
        _oven.setDutyCycle(_value,true);

        // something happened

        _lastActionTime=MillisecondTimer::millis();
      }

      // have we timed out?

      if(MillisecondTimer::hasTimedOut(_lastActionTime,INACTION_TIMEOUT)) {
        _oven.setDutyCycle(0,true);
        return ProgramState::REMOTING;
      }

    } while(!_actionButton.isPressed());

    // oven off

    _oven.setDutyCycle(0,true);

    // save the new state

    Eeprom::Writer::zeroPercentage(_value);

    // move back to the remoting state

    return ProgramState::REMOTING;
  }
}
