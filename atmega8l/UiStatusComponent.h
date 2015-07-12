/*
 * Andy's Workshop Reflow Controller ATMega328p firmware
 * Copyright (c) 2015 Andy Brown. http://www.andybrown.me.uk
 * Please see website for licensing terms.
 */

#pragma once


namespace awreflow {


  /*
   * display a status line at the bottom of the screen
   */

  class UiStatusComponent {
    
    protected:
      uint8_t _currentIndex;
      const char * const*_stringTable;

    public:
      UiStatusComponent(const char *const *stringTable);

      void loop(uint8_t newIndex);
  };


  /*
   * Constructor
   */

  inline UiStatusComponent::UiStatusComponent(const char *const *stringTable) {
    _stringTable=stringTable;
    _currentIndex=0xff;
  }


  /*
   * Main loop
   */

  inline void UiStatusComponent::loop(uint8_t newIndex) {

    // nothing to do?

    if(newIndex==_currentIndex)
      return;

    _currentIndex=newIndex;
    Nokia5110::writeString_P(0,5,(PGM_P)pgm_read_word(&_stringTable[newIndex]),false);
  }
}
