// Andy's Workshop Open Source Reflow Controller
// Copyright (c) 2015 Andy Brown. All rights Reserved.
// Please see website (http://www.andybrown.me.uk) for full license details.

package uk.me.andybrown.awreflow2;

import org.apache.commons.lang3.StringUtils;

import java.util.Arrays;
import java.util.Iterator;


/*
 * Reflow profile class
 */

public class ReflowProfile implements Iterable<ReflowRegion> {

  protected String _name;               // the name is the unique key
  protected ReflowRegion[] _regions;

  protected double _linearMaxTemperature;
  protected double _curveMaxTemperature;

  protected double[] _curvePoints;      // one point per second
  protected double[] _linearPoints;     // ditto


  /*
   * Get the string title
   */

  @Override
  public String toString() {
    return _name;
  }


  /*
   * Constructor
   */

  public ReflowProfile(String name,ReflowRegion[] regions,double[] curvePoints) {

    int i,endTime;
    double x,y,ya,yb,xa,xb;

    // store values

    _name=name;
    _regions=regions;
    _curvePoints=curvePoints;

    // calc the maximum temperatures

    _linearMaxTemperature=0;
    _curveMaxTemperature=0;

    for(i=0;i<regions.length;i++)
      _linearMaxTemperature=Math.max(regions[i].getEndTemperature(),_linearMaxTemperature);

    for(i=0;i<_curvePoints.length;i++)
      _curveMaxTemperature=Math.max(_curvePoints[i],_curveMaxTemperature);

    // calc the linear points

    endTime=(int)regions[regions.length-1].getEndTime();
    _linearPoints=new double[endTime+1];

    xb=0;
    yb=25;
    x=0;

    for(ReflowRegion rr : _regions) {

      // previous end becomes the start

      xa=xb;
      ya=yb;

      // new end from the region

      xb=rr.getEndTime();
      yb=rr.getEndTemperature();

      do {

        // solve for y along a straight line between the two points

        y=(((yb-ya)/(xb-xa))*(x-xa))+ya;

        _linearPoints[(int)x]=y;

        x++;
      } while(x<=rr.getEndTime());
    }
  }


  /*
   * Override the equals method
   */

  @Override
  public boolean equals(Object obj) {

    if(obj==null)
      return false;

    if(obj==this)
      return true;

    if(obj.getClass()!=getClass())
      return false;

    return StringUtils.equalsIgnoreCase(((ReflowProfile)obj)._name, _name);
  }


  /*
   * Get the number of regions
   */

  public int getRegionCount() {
    return _regions.length;
  }


  /*
   * Get an indexed region
   */

  public ReflowRegion getAt(int pos) {
    return _regions[pos];
  }


  /*
   * Get the max temperature
   */

  public double getMaxTemperature(TrackingType tt) {
    return tt==TrackingType.LINEAR ? _linearMaxTemperature : _curveMaxTemperature;
  }


  /*
   * Get the point array
   */

  public double[] getPoints(TrackingType tt) {
    return tt==TrackingType.LINEAR ? _linearPoints : _curvePoints;
  }


  /*
   * HashCode override
   */

  @Override
  public int hashCode() {
    return _name.hashCode();
  }


  /*
   * Handy conversion to an iterator
   */

  @Override
  public Iterator<ReflowRegion> iterator() {
    return Arrays.asList(_regions).iterator();
  }
}
