/*
 * Andy's Workshop Reflow Controller ATMega328p firmware
 * Copyright (c) 2015 Andy Brown. http://www.andybrown.me.uk
 * Please see website for licensing terms.
 */

#pragma once


namespace awreflow {


  /*
   * UI for the profile selection screen. A menu is used to select
   * the leaded/unleaded option.
   */

  struct UiSelectingProfileDisplay : UiBannerComponent,UiMenuComponent {
    UiSelectingProfileDisplay(Encoder& encoder,ActionButton& actionButton);
  };


  /*
   * Constructor
   */

  inline UiSelectingProfileDisplay::UiSelectingProfileDisplay(Encoder& encoder,ActionButton& actionButton)
    : UiBannerComponent(SelectingProfileString),
      UiMenuComponent(encoder,
                      actionButton,
                      SelectingProfileStringTable,
                      sizeof(SelectingProfileStringTable)/sizeof(SelectingProfileStringTable[0]),
                      Eeprom::Location::PROFILE) {
  }
}
