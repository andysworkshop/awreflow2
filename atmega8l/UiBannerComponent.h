/*
 * Andy's Workshop Reflow Controller ATMega328p firmware
 * Copyright (c) 2015 Andy Brown. http://www.andybrown.me.uk
 * Please see website for licensing terms.
 */

#pragma once


namespace awreflow {


  /*
   * display an inverted banner at the top of the screen
   */

  struct UiBannerComponent {
    UiBannerComponent(const char *banner_p);
  };


  /*
   * Constructor. The banner display is a one-time job done in the constructor
   */

  inline UiBannerComponent::UiBannerComponent(const char *banner_p) {
    Nokia5110::writeString_P(0,0,banner_p,true);
  }
}
