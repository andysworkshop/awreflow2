/*
 * Andy's Workshop Reflow Controller ATMega328p firmware
 * Copyright (c) 2015 Andy Brown. http://www.andybrown.me.uk
 * Please see website for licensing terms.
 */

#pragma once


namespace awreflow {

  /*
   * Handler for the reflowing state
   */

  class ReflowReadyHandler {

    protected:
      UiReflowReadyDisplay _display;

    public:
      ReflowReadyHandler(Encoder& encoder,ActionButton& actionButton);

      ProgramState loop();
  };


  /*
   * Constructor
   */

  inline ReflowReadyHandler::ReflowReadyHandler(Encoder& encoder,ActionButton& actionButton) 
    : _display(encoder,actionButton) {
  }


  /*
   * Main loop
   */

  inline ProgramState ReflowReadyHandler::loop() {

    uint8_t selectedItem;

    do {

      // loop the display and check for timeout

      if(!_display.loop())
        return ProgramState::REMOTING;

    } while(!_display.getSelectedItem(selectedItem));

    // move to reflowing or abort

    if(selectedItem==0)
      return ProgramState::REFLOWING;

    // cancelled

    return ProgramState::REMOTING;
  }
}
