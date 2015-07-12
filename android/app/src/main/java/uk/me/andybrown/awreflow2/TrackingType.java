// Andy's Workshop Open Source Reflow Controller
// Copyright (c) 2015 Andy Brown. All rights Reserved.
// Please see website (http://www.andybrown.me.uk) for full license details.

package uk.me.andybrown.awreflow2;

/*
 * Type of tracking
 */

public enum TrackingType {

  LINEAR(0),
  SPLINE_CURVE(1);

  protected int _value;

  TrackingType(int value) {
    _value=value;
  }

  public int getValue() {
    return _value;
  }

  static TrackingType fromValue(int value) {
    if(value==0)
      return TrackingType.LINEAR;
    else
      return TrackingType.SPLINE_CURVE;
  }
}
