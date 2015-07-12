/*
 * Andy's Workshop Reflow Controller ATMega328p firmware
 * Copyright (c) 2015 Andy Brown. http://www.andybrown.me.uk
 * Please see website for licensing terms.
 */

#pragma once

namespace awreflow {


  /*
   * Handler for the LCD options state
   */

  class LcdOptionsHandler {

    protected:
      UiLcdOptionsDisplay _display;

      enum {
        BACKLIGHT = 0,
        CONTRAST = 1
      };

    public:
      LcdOptionsHandler(Encoder& encoder,ActionButton& actionButton);

      ProgramState loop();
  };


  /*
   * Constructor
   */

  inline LcdOptionsHandler::LcdOptionsHandler(Encoder& encoder,ActionButton& actionButton)
    : _display(encoder,actionButton) {
  }


  /*
   * Main loop
   */

  inline ProgramState LcdOptionsHandler::loop() {

    uint8_t selectedItem;

    do {

      // loop the display and check for timeout

      if(!_display.loop())
        return ProgramState::REMOTING;

    } while(!_display.getSelectedItem(selectedItem));

    // move to the selected state

    switch(selectedItem) {
      
      case BACKLIGHT:
        return ProgramState::LCD_BACKLIGHT;

      case CONTRAST:
      default:
        return ProgramState::LCD_CONTRAST;
        break;
    }
  }
}
