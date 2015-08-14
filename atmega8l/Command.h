/*
 * Andy's Workshop Reflow Controller ATMega328p firmware
 * Copyright (c) 2015 Andy Brown. http://www.andybrown.me.uk
 * Please see website for licensing terms.
 */

#pragma once


namespace awreflow {

  /*
   * Possible commands
   */

  enum class Command : uint8_t {
    NONE              = 0,
    READ_TEMPERATURE  = 1,
    SET_DUTY_CYCLE    = 2,
    SET_SENSOR_OFFSET = 3,
    SET_LCD_BACKLIGHT = 4,
    SET_LCD_CONTRAST  = 5,
    READ_SETTINGS     = 6
  };


  /*
   * Possible ack/nack
   */

  enum class Acknowledge : uint8_t {
    ACK   = 0,
    NACK  = 1
  };
}
