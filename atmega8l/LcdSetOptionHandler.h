/*
 * Andy's Workshop Reflow Controller ATMega328p firmware
 * Copyright (c) 2015 Andy Brown. http://www.andybrown.me.uk
 * Please see website for licensing terms.
 */

#pragma once


namespace awreflow {


  /*
   * Handler for the backlight percentage/contrast value. Encoder +/- is used to select the
   * new value and store it in the EEPROM.
   */

  class LcdSetOptionHandler {

    public:

    protected:
      Encoder& _encoder;
      ActionButton& _actionButton;
      uint8_t _maxValue;
      Eeprom::Location _eepromLocation;

      UiLcdSetOptionDisplay _display;

      uint8_t _value;
      uint32_t _lastActionTime;

      enum {
        INACTION_TIMEOUT = 10000
      };

    protected:
      void setDisplay();

    public:
      LcdSetOptionHandler(Encoder& encoder,ActionButton& actionButton,uint8_t maxValue,Eeprom::Location eepromLocation,const char *banner);

      ProgramState loop();
  };


  /*
   * Constructor
   */

  inline LcdSetOptionHandler::LcdSetOptionHandler(Encoder& encoder,ActionButton& actionButton,uint8_t maxValue,Eeprom::Location eepromLocation,const char *banner) 
    : _encoder(encoder),
      _actionButton(actionButton),
      _maxValue(maxValue),
      _eepromLocation(eepromLocation),
      _display(banner) {

    // read the value from EEPROM

    _value=Eeprom::Reader::readByte(_eepromLocation);

    // set the LCD from the value

    setDisplay();

    // set the last action timeout to now

    _lastActionTime=MillisecondTimer::millis();
  }


  /*
   * Main loop
   */

  inline ProgramState LcdSetOptionHandler::loop() {

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
          _value=utils::min<uint8_t>(_maxValue,_value+movement);

        // set the new value to the LCD

        setDisplay();

        // something happened

        _lastActionTime=MillisecondTimer::millis();
      }

      // have we timed out?

      if(MillisecondTimer::hasTimedOut(_lastActionTime,INACTION_TIMEOUT))
        return ProgramState::REMOTING;

    } while(!_actionButton.isPressed());

    // save the new state

    Eeprom::Writer::writeByte(_eepromLocation,_value);

    // move back to the remoting state

    return ProgramState::REMOTING;
  }


  /*
   * Set a new value to the display
   */

  inline void LcdSetOptionHandler::setDisplay() {

    _display.setNumber(_value);

    // set the new value to the LCD

    if(_eepromLocation==Eeprom::Location::LCD_BACKLIGHT)
      Nokia5110::setBacklight(_value);
    else
      Nokia5110::setContrast(_value);
  }
}
