/*
 * Andy's Workshop Reflow Controller ATMega328p firmware
 * Copyright (c) 2015 Andy Brown. http://www.andybrown.me.uk
 * Please see website for licensing terms.
 */

#pragma once

namespace awreflow {


  /*
   * Handler for the profile selecting state
   */

  class SelectingProfileHandler {

    protected:
      UiSelectingProfileDisplay _display;
      ReflowProfileName _profileName;

    public:
      SelectingProfileHandler(Encoder& encoder,ActionButton& actionButton);

      ProgramState loop();
  };


  /*
   * Constructor
   */

  inline SelectingProfileHandler::SelectingProfileHandler(Encoder& encoder,ActionButton& actionButton) 
    : _display(encoder,actionButton) {
  }


  /*
   * Main loop
   */

  inline ProgramState SelectingProfileHandler::loop() {

    uint8_t selectedItem;

    do {

      // loop the display and check for timeout

      if(!_display.loop())
        return ProgramState::REMOTING;

    } while(!_display.getSelectedItem(selectedItem));

    // move to reflow ready

    return ProgramState::REFLOW_READY;
  }
}
