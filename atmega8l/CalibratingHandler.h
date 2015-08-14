/*
 * Andy's Workshop Reflow Controller ATMega328p firmware
 * Copyright (c) 2015 Andy Brown. http://www.andybrown.me.uk
 * Please see website for licensing terms.
 */

#pragma once


namespace awreflow {


  /*
   * Handler for the temperature sensor offset calibration. Encoder +/- is used to select the
   * new value and store it in the EEPROM.
   */

  class CalibratingHandler {

    public:

    protected:
      Encoder& _encoder;
      ActionButton& _actionButton;

      UiCalibratingDisplay _display;

      int8_t _value;
      uint32_t _lastActionTime;

      enum {
        INACTION_TIMEOUT = 30000
      };

    public:
      CalibratingHandler(Encoder& encoder,ActionButton& button);

      ProgramState loop();
  };


  /*
   * Constructor
   */

  inline CalibratingHandler::CalibratingHandler(Encoder& encoder,ActionButton& actionButton) 
    : _encoder(encoder),
      _actionButton(actionButton) {

    // read the value from EEPROM

    _value=Eeprom::Reader::sensorOffset();
    _display.setSignedNumber(_value);

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

        _value+=movement;
        if(_value>99)
          _value=99;
        else if(_value<-99)
          _value=-99;

        // set the new value to the display 

        _display.setSignedNumber(_value);

        // something happened

        _lastActionTime=MillisecondTimer::millis();
      }

      // have we timed out?

      if(MillisecondTimer::hasTimedOut(_lastActionTime,INACTION_TIMEOUT)) {
        return ProgramState::REMOTING;
      }

    } while(!_actionButton.isPressed());

    // save the new state

    Eeprom::Writer::sensorOffset(_value);

    // move back to the remoting state

    return ProgramState::REMOTING;
  }
}
