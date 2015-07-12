/*
 * Andy's Workshop Reflow Controller ATMega328p firmware
 * Copyright (c) 2015 Andy Brown. http://www.andybrown.me.uk
 * Please see website for licensing terms.
 */

#pragma once


namespace awreflow {

  /*
   * Class to handle a simple menu of choices
   */

  class UiMenuComponent {

    protected:
      Encoder& _encoder;
      ActionButton& _actionButton;
      Eeprom::Location _eepromLocation;

      const char * const*_stringTable;
      uint8_t _stringCount;
      uint8_t _selectedItem;
      uint32_t _lastActionTime;

      enum {
        INACTIVITY_TIMEOUT = 5000
      };

    protected:
      void redraw();

    public:
      UiMenuComponent(Encoder& encoder,ActionButton& actionButton,const char *const *stringTable,uint8_t stringCount,Eeprom::Location eepromLocation);

      bool loop();
      bool getSelectedItem(uint8_t& selectedItem) const;
  };


  /*
   * Constructor
   */

  inline UiMenuComponent::UiMenuComponent(Encoder& encoder,ActionButton& actionButton,const char *const *stringTable,uint8_t stringCount,Eeprom::Location eepromLocation)
    : _encoder(encoder),
      _actionButton(actionButton) {

    _stringTable=stringTable;
    _stringCount=stringCount;
    _eepromLocation=eepromLocation;
    
    // read the last selection from EEPROM and use it as the default 

    _selectedItem=Eeprom::Reader::readByte(eepromLocation);
    if(_selectedItem>=stringCount)
      _selectedItem=0;

    _lastActionTime=MillisecondTimer::millis();

    redraw();
  }


  /*
   * Main loop: return false if timed out
   */

  inline bool UiMenuComponent::loop() {

    int8_t movement;

    // check for inactivity timeout

    if(MillisecondTimer::hasTimedOut(_lastActionTime,INACTIVITY_TIMEOUT))
      return false;

    // call the loops

    _encoder.loop();
    _actionButton.loop();

    // check for encoder movement

    if((movement=_encoder.getChange())==0)
      return true;

    // update the position

    if(movement<0 && -movement>_selectedItem)
      _selectedItem=0;
    else if(movement>0 && movement+_selectedItem>=_stringCount)
      _selectedItem=_stringCount-1;
    else
      _selectedItem+=movement;

    // update the last action time

    _lastActionTime=MillisecondTimer::millis();

    // redraw the screen

    redraw();
    return true;
  }


  /*
   * Get the currently selected item and return true if the
   * button has been pressed
   */

  inline bool UiMenuComponent::getSelectedItem(uint8_t& selectedItem) const {

    selectedItem=_selectedItem;
    
    if(_actionButton.isPressed()) {
      
      // button pressed, store the selection in EEPROM

      Eeprom::Writer::writeByte(_eepromLocation,_selectedItem);
      return true;
    }

    // nothing pressed

    return false;
  }


  /*
   * Redraw the menu
   */

  inline void UiMenuComponent::redraw() {

    uint8_t i,y;
    char buffer[13];

    y=2;
    for(i=0;i<_stringCount;i++,y++) {

      strcpy_P(buffer,(PGM_P)pgm_read_word(&_stringTable[i]));
      Nokia5110::writeString(0,y,buffer,i==_selectedItem);
    }
  }
}
