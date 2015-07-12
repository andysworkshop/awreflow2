/*
 * Andy's Workshop Reflow Controller ATMega328p firmware
 * Copyright (c) 2015 Andy Brown. http://www.andybrown.me.uk
 * Please see website for licensing terms.
 */

#pragma once


namespace awreflow {


  /*
   * UI for the LCD set options (backlight, contrast) handler. Show a banner title
   * and a large number being adjusted.
   */

  struct UiLcdSetOptionDisplay : UiBannerComponent, UiLargeNumberComponent {
    UiLcdSetOptionDisplay(const char *banner);
  };


  /*
   * Constructor
   */

  inline UiLcdSetOptionDisplay::UiLcdSetOptionDisplay(const char *banner)
    : UiBannerComponent(banner),
      UiLargeNumberComponent(0,2) {
  }
}
