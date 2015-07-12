/*
 * Andy's Workshop Reflow Controller ATMega328p firmware
 * Copyright (c) 2015 Andy Brown. http://www.andybrown.me.uk
 * Please see website for licensing terms.
 */

#pragma once


namespace awreflow {


  /*
   * Main menu: reflow/pid/calibrate/LCD
   */

  struct UiSelectingModeDisplay : UiBannerComponent,UiMenuComponent {
    UiSelectingModeDisplay(Encoder& encoder,ActionButton& actionButton);
  };


  /*
   * Constructor
   */

  inline UiSelectingModeDisplay::UiSelectingModeDisplay(Encoder& encoder,ActionButton& actionButton)
    : UiBannerComponent(SelectingModeString),
      UiMenuComponent(encoder,
                      actionButton,
                      SelectingModeStringTable,
                      sizeof(SelectingModeStringTable)/sizeof(SelectingModeStringTable[0]),
                      Eeprom::Location::MODE) {
  }
}
