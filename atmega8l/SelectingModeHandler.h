/*
 * Andy's Workshop Reflow Controller ATMega328p firmware
 * Copyright (c) 2015 Andy Brown. http://www.andybrown.me.uk
 * Please see website for licensing terms.
 */

#pragma once

namespace awreflow {


  /*
   * Handler for the mode selecting state
   */

  class SelectingModeHandler {

    protected:
      UiSelectingModeDisplay _display;

      enum {
        MODE_REFLOW = 0,
        MODE_PID = 1,
        MODE_CALIBRATE =2,
        MODE_LCD = 3
      };

    public:
      SelectingModeHandler(Encoder& encoder,ActionButton& actionButton);

      ProgramState loop();
  };


  /*
   * Constructor
   */

  inline SelectingModeHandler::SelectingModeHandler(Encoder& encoder,ActionButton& actionButton)
    : _display(encoder,actionButton) {
  }


  /*
   * Main loop
   */

  inline ProgramState SelectingModeHandler::loop() {

    uint8_t selectedItem;

    do {

      // loop the display and check for timeout

      if(!_display.loop())
        return ProgramState::REMOTING;

    } while(!_display.getSelectedItem(selectedItem));

    // move to the selected state

    switch(selectedItem) {
      
      case MODE_REFLOW:
        return ProgramState::SELECTING_PROFILE;

      case MODE_PID:
        return ProgramState::SELECTING_PROPORTIONAL;

      case MODE_LCD:
        return ProgramState::LCD_OPTIONS;

      case MODE_CALIBRATE:
      default:
        return ProgramState::CALIBRATING;
    }
  }
}
