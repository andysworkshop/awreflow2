// Andy's Workshop Open Source Reflow Controller
// Copyright (c) 2015 Andy Brown. All rights Reserved.
// Please see website (http://www.andybrown.me.uk) for full license details.

package uk.me.andybrown.awreflow2;

/*
 * Activity codes
 */

public enum ActivityCodes {

  REQUEST_ENABLE_BT(1);

  private int _value;

  ActivityCodes(int value) {
    _value = value;
  }

  public int getValue() {
    return _value;
  }
}
