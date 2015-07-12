/*
 * Andy's Workshop Reflow Controller ATMega328p firmware
 * Copyright (c) 2015 Andy Brown. http://www.andybrown.me.uk
 * Please see website for licensing terms.
 */

#pragma once


namespace awreflow {


  /*
   * LCD options: backlight, contrast
   */

  struct UiLcdOptionsDisplay : UiBannerComponent,UiMenuComponent {
    UiLcdOptionsDisplay(Encoder& encoder,ActionButton& actionButton);
  };


  /*
   * Constructor
   */

  inline UiLcdOptionsDisplay::UiLcdOptionsDisplay(Encoder& encoder,ActionButton& actionButton)
    : UiBannerComponent(LcdOptionsString),
      UiMenuComponent(encoder,
                      actionButton,
                      LcdOptionsStringTable,
                      sizeof(LcdOptionsStringTable)/sizeof(LcdOptionsStringTable[0]),
                      Eeprom::Location::LCD_OPTIONS) {
  }
}
