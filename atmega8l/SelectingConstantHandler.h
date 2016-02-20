/*
 * Andy's Workshop Reflow Controller ATMega328p firmware
 * Copyright (c) 2015 Andy Brown. http://www.andybrown.me.uk
 * Please see website for licensing terms.
 */

#pragma once


namespace awreflow {


  /*
   * Handler for the constant selecting state. Encoder +/- is used to select the
   * new value and store it in the EEPROM.
   */

  class SelectingConstantHandler {

    public:

      /*
       * Encapsulate the values that define uniquely how this class will operate. Must be POD values.
       */

      struct Config {
        Eeprom::Location location;
        ProgramState nextState;
        const char *banner;
      };

    protected:
      Encoder& _encoder;
      ActionButton& _actionButton;

      Config _config;
      UiSelectingConstantDisplay _display;

      uint16_t _value;
      uint32_t _lastActionTime;

      enum {
        INACTION_TIMEOUT = 5000,
        MAXIMUM_VALUE    = 999
      };

    public:
      SelectingConstantHandler(Encoder& encoder,ActionButton& button,Config config);

      ProgramState loop();
  };


  /*
   * Constructor
   */

  inline SelectingConstantHandler::SelectingConstantHandler(Encoder& encoder,ActionButton& actionButton,Config config) 
    : _encoder(encoder),
      _actionButton(actionButton),
      _config(config),
      _display(config.banner) {

    // read the value from EEPROM

    _value=Eeprom::Reader::constant(config.location);
    _display.setNumber(_value);

    // set the last action timeout to now

    _lastActionTime=MillisecondTimer::millis();
  }


  /*
   * Main loop
   */

  inline ProgramState SelectingConstantHandler::loop() {

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
          _value=utils::min<uint16_t>(MAXIMUM_VALUE,_value+movement);

        // set the new value to the display

        _display.setNumber(_value);

        // something happened

        _lastActionTime=MillisecondTimer::millis();
      }

      // have we timed out?

      if(MillisecondTimer::hasTimedOut(_lastActionTime,INACTION_TIMEOUT))
        return ProgramState::REMOTING;

    } while(!_actionButton.isPressed());

    // save the new state

    Eeprom::Writer::constant(_config.location,_value);

    // move to the next configured state

    return _config.nextState;
  }
}
