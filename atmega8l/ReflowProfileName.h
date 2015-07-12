/*
 * Andy's Workshop Reflow Controller ATMega328p firmware
 * Copyright (c) 2015 Andy Brown. http://www.andybrown.me.uk
 * Please see website for licensing terms.
 */


#pragma once


namespace awreflow {

  /*
   * Reflow profile names - the numbers match up with the position in the
   * on-screen menu.
   */
   
  enum class ReflowProfileName : uint8_t {
    LEADED    = 0,
    LEAD_FREE = 1
  };
}