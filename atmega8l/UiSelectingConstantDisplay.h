/*
 * Andy's Workshop Reflow Controller ATMega328p firmware
 * Copyright (c) 2015 Andy Brown. http://www.andybrown.me.uk
 * Please see website for licensing terms.
 */

#pragma once


namespace awreflow {


  /*
   * Selecting a constant UI handler. There's a banner component on the top
   * and the constant as a large number below.
   */

  struct UiSelectingConstantDisplay : UiBannerComponent, UiLargeNumberComponent {
    UiSelectingConstantDisplay(const char *banner);
  };


  /*
   * Constructor
   */

  inline UiSelectingConstantDisplay::UiSelectingConstantDisplay(const char *banner)
    : UiBannerComponent(banner),
      UiLargeNumberComponent(0,2) {
  }
}
