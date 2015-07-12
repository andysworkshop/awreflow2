// Andy's Workshop Open Source Reflow Controller
// Copyright (c) 2015 Andy Brown. All rights Reserved.
// Please see website (http://www.andybrown.me.uk) for full license details.

package uk.me.andybrown.awreflow2;

/*
 * Created by Andy on 29/03/2015.
 */

public enum CommandId {

  READ_TEMPERATURE(1),
  SET_DUTY_CYCLE(2),
  SET_OVEN_ZERO(3),
  SET_LCD_BACKLIGHT(4),
  SET_LCD_CONTRAST(5),
  READ_SETTINGS(6);

  private byte _id;

  CommandId(int id) {
    _id=(byte)id;
  }

  byte getId() {
    return _id;
  }
}
