/*
 * Andy's Workshop Reflow Controller ATMega328p firmware
 * Copyright (c) 2015 Andy Brown. http://www.andybrown.me.uk
 * Please see website for licensing terms.
 */

#pragma once


namespace awreflow {


  /*
   * UI for the calibration handler. Display a title with a large number
   * below it that indicates the calibration handler.
   */

  struct UiCalibratingDisplay : UiBannerComponent, UiLargeNumberComponent {
    UiCalibratingDisplay();
  };


  /*
   * Constructor
   */

  inline UiCalibratingDisplay::UiCalibratingDisplay()
    : UiBannerComponent(CalibratingString),
      UiLargeNumberComponent(0,2) {
  }
}
