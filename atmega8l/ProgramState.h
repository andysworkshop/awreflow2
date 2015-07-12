/*
 * Andy's Workshop Reflow Controller ATMega328p firmware
 * Copyright (c) 2015 Andy Brown. http://www.andybrown.me.uk
 * Please see website for licensing terms.
 */


#pragma once


namespace awreflow {

  /*
   * Current program state
   */

  enum class ProgramState : uint8_t { 
    REMOTING,                   // receiving bluetooth, showing temperature
    SELECTING_MODE,             // selecting reflow/pid/cal
    CALIBRATING,                // calibrating zero percentage
    LCD_OPTIONS,                // LCD backlight/contrast
    LCD_BACKLIGHT,              // LCD backlight
    LCD_CONTRAST,               // LCD contrast
    SELECTING_PROFILE,          // choosing leaded/unleaded
    SELECTING_PROPORTIONAL,     // selecting proportional co-efficient
    SELECTING_INTEGER,          // selecting integer co-efficient
    SELECTING_DERIVATIVE,       // selecting derivative co-efficient
    REFLOW_READY,               // selecting ready/abort
    REFLOWING                   // reflowing
  };
}
