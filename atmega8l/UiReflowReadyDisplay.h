/*
 * Andy's Workshop Reflow Controller ATMega328p firmware
 * Copyright (c) 2015 Andy Brown. http://www.andybrown.me.uk
 * Please see website for licensing terms.
 */

#pragma once


namespace awreflow {


  /*
   * UI for the reflow ready display. Shows a menu with "Reflow now" and "Cancel"
   */

  struct UiReflowReadyDisplay : UiBannerComponent,UiMenuComponent {
    UiReflowReadyDisplay(Encoder& encoder,ActionButton& actionButton);
  };


  /*
   * Constructor
   */

  inline UiReflowReadyDisplay::UiReflowReadyDisplay(Encoder& encoder,ActionButton& actionButton)
    : UiBannerComponent(ReflowReadyString),
      UiMenuComponent(encoder,
                      actionButton,
                      ReflowReadyStringTable,
                      sizeof(ReflowReadyStringTable)/sizeof(ReflowReadyStringTable[0]),
                      Eeprom::Location::REFLOW_READY) {
  }
}
