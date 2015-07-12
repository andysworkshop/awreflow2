// Andy's Workshop Open Source Reflow Controller
// Copyright (c) 2015 Andy Brown. All rights Reserved.
// Please see website (http://www.andybrown.me.uk) for full license details.

package uk.me.andybrown.awreflow2;

/*
 * The reflow region class encapsulates a region on the profile, for example
 * "preheat", "soak" etc.
 */

public class ReflowRegion {

  protected String _name;               // name of the reflow region
  protected double _endTime;            // ending time in seconds
  protected double _endTemperature;     // ending temperature


  /**
   * Constructor
   * @param name The name of the region
   * @param etime The ending time
   * @param etemp The ending temp
   */

  public ReflowRegion(String name,double etime,double etemp) {
    _name=name;
    _endTime=etime;
    _endTemperature=etemp;
  }


  /*
   * Getters
   */

  public String getName() { return _name; }
  public double getEndTemperature() { return _endTemperature; }
  public double getEndTime() {  return _endTime; }
}
