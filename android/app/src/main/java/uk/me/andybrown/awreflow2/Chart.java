// Andy's Workshop Open Source Reflow Controller
// Copyright (c) 2015 Andy Brown. All rights Reserved.
// Please see website (http://www.andybrown.me.uk) for full license details.

package uk.me.andybrown.awreflow2;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.DashPathEffect;
import android.graphics.LinearGradient;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.Point;
import android.graphics.Shader;
import android.graphics.Typeface;
import android.util.AttributeSet;
import android.view.View;

import java.util.Map;
import java.util.TreeMap;


/*
 * Custom view class for drawing the chart
 */

public class Chart extends View {

  protected static final int LINE_WIDTH = 3;
  protected static final int AXIS_WIDTH = 2;
  protected static final int NAME_HEIGHT = 16;
  protected static final int NUMBERS_HEIGHT = 14;

  protected ReflowProfile _profile=new LeadReflowProfile();
  protected TrackingType _trackingType=TrackingType.SPLINE_CURVE;

  protected int _width;
  protected int _height;

  protected TreeMap<Integer,Double> _progressValues=new TreeMap<>();

  protected Paint _axisPaint;
  protected Paint _numbersPaint;
  protected Paint _linePaint;
  protected Paint _regionLinePaint;
  protected Paint _namePaint;
  protected Paint _progressPaint;

  protected Point _origin;


  /*
   * Constructors
   */

  public Chart(Context context) {
    super(context);
    init();
  }

  public Chart(Context context,AttributeSet attrs) {
    super(context,attrs);
    init();
  }

  protected void init() {

    // create the graphics objects

    _axisPaint=new Paint(Paint.ANTI_ALIAS_FLAG);
    _axisPaint.setColor(0xFF404040);
    _axisPaint.setStrokeWidth(AXIS_WIDTH);
    _axisPaint.setStyle(Paint.Style.STROKE);

    _numbersPaint=new Paint(Paint.ANTI_ALIAS_FLAG);
    _numbersPaint.setColor(0xFF808080);
    _numbersPaint.setTextSize(NUMBERS_HEIGHT);

    _linePaint=new Paint(Paint.ANTI_ALIAS_FLAG);
    _linePaint.setColor(0xFF808080);
    _linePaint.setStrokeWidth(LINE_WIDTH);
    _linePaint.setStyle(Paint.Style.STROKE);

    _progressPaint=new Paint(Paint.ANTI_ALIAS_FLAG);
    _progressPaint.setColor(0xFF00F000);
    _progressPaint.setStrokeWidth(LINE_WIDTH);
    _progressPaint.setStyle(Paint.Style.STROKE);

    _regionLinePaint=new Paint(Paint.ANTI_ALIAS_FLAG);
    _regionLinePaint.setColor(0xFF808080);
    _regionLinePaint.setStrokeWidth(1);
    _regionLinePaint.setStyle(Paint.Style.STROKE);
    _regionLinePaint.setPathEffect(new DashPathEffect(new float[] { 1, 2 },0));

    _namePaint=new Paint(Paint.ANTI_ALIAS_FLAG);
    _namePaint.setColor(0xFF000000);
    _namePaint.setTextSize(NAME_HEIGHT);
    _namePaint.setTypeface(Typeface.defaultFromStyle(Typeface.ITALIC));
  }


  /*
   * clear down the progress
   */

  public void clearProgress() {
    _progressValues.clear();
  }


  /*
   * Update progress with a later time and temperature than
   * the last one that was received
   */

  public void updateProgress(int seconds,double temperature) {
    _progressValues.put(seconds,temperature);
  }


  /*
   * Set the constant measurements
   */

  protected void setSizes() {

    float width;

    // calculate the largest text width of the reflow region points

    width=_numbersPaint.measureText("25");

    for(ReflowRegion rr : _profile)
      width=Math.max(width,_numbersPaint.measureText(Integer.toString((int)rr.getEndTemperature())));

    // tack on 10dp either side of the text plus 2 for the axis

    width+=20+AXIS_WIDTH;

    // set the origin

    _origin=new Point((int)width,_height-(10+NUMBERS_HEIGHT+10+AXIS_WIDTH));
  }


  /*
   * Canvas size changed
   */

  @Override
  protected void onSizeChanged(int w,int h,int oldw,int oldh) {

    // call the base class

    super.onSizeChanged(w,h,oldw,oldh);

    // cache the sizes

    _width=w;
    _height=h;

    // calculate sizes

    setSizes();
  }


  /*
   * Draw the view
   */

  @Override
  protected void onDraw(Canvas canvas) {

    double[] points;

    // call the base class

    super.onDraw(canvas);

    // get the points array for the tracking type

    points=_profile.getPoints(_trackingType);

    // do the drawing

    drawAxes(canvas);
    fillBelowCurve(canvas,points);
    plotIdealPath(canvas,points);
    plotProgressPath(canvas);
  }


  /*
   * Draw the progress path on the chart
   */

  protected void plotProgressPath(Canvas canvas) {

    Point p;
    Path path;
    boolean first;

    // check for not started

    if(_progressValues.size()==0)
      return;

    // plot the points on the curve into a path object

    path=new Path();

    // iterate the map of seconds to temperature

    first=true;
    for(Map.Entry<Integer,Double> e : _progressValues.entrySet()) {

      // add the point to the path

      p=reflowPointToChart(e.getKey(),e.getValue());

      if(first) {
        path.moveTo(p.x,p.y);
        first=false;
      }
      else
        path.lineTo(p.x,p.y);
    }

    // draw the path on to the canvas

    canvas.drawPath(path,_progressPaint);
  }


  /*
   * Draw the ideal line on the chart
   */

  protected void plotIdealPath(Canvas canvas,double[] points) {

    Point p,startPoint;
    Path path;
    int i,x,y,regionIndex,nameWidth;
    String name;

    // plot the points on the curve into a path object

    path=new Path();
    p=reflowPointToChart(0,points[0]);
    path.moveTo(p.x,p.y);
    regionIndex=0;
    startPoint=p;

    for(i=1;i<points.length;i++) {

      // add the point to the path

      p=reflowPointToChart(i,points[i]);
      path.lineTo(p.x,p.y);

      if(i==_profile.getAt(regionIndex).getEndTime()) {

        // draw the dotted line up and down to the axis

        Path linePath=new Path();

        linePath.moveTo(_origin.x,p.y);
        linePath.lineTo(p.x,p.y);
        linePath.lineTo(p.x,_origin.y);
        canvas.drawPath(linePath,_regionLinePaint);

        // draw the name

        name=_profile.getAt(regionIndex).getName();
        nameWidth=(int)_namePaint.measureText(name);

        if(nameWidth+10<p.x-startPoint.x)
          canvas.drawText(name,startPoint.x+(((p.x-startPoint.x)/2)-(nameWidth/2)),_origin.y-10,_namePaint);
        else {

          x=startPoint.x+(((p.x-startPoint.x)/2)-(NAME_HEIGHT/2));
          y=_origin.y-10-nameWidth;

          canvas.save();
          canvas.rotate(90,x,y);
          canvas.drawText(name,x,y,_namePaint);
          canvas.restore();
        }
        startPoint=p;
        regionIndex++;
      }
    }

    // draw the path on to the canvas

    canvas.drawPath(path,_linePaint);
  }


  /*
   * Draw the axes
   */

  protected void drawAxes(Canvas canvas) {
    // draw the axis lines

    canvas.drawLine(_origin.x-AXIS_WIDTH,0,_origin.x,_origin.y+AXIS_WIDTH,_axisPaint);   // Y
    canvas.drawLine(_origin.x,_origin.y+1,_width,_origin.y+1,_axisPaint);     // X

    // draw the X axis values

    for(ReflowRegion rr : _profile) {
      drawXValue(rr.getEndTime(),canvas);
      drawYValue(rr.getEndTemperature(),canvas);
    }
  }


  /*
   * Fill the area below the canvas
   */

  protected void fillBelowCurve(Canvas canvas,double[] points) {

    Point p,startPoint;
    Path path;
    int i,state;

    path=new Path();
    startPoint=reflowPointToChart(0,points[0]);
    path.moveTo(startPoint.x,startPoint.y);

    state=0;

    for(i=1;i<points.length;i++) {

      // add the point to the path

      p=reflowPointToChart(i,points[i]);
      path.lineTo(p.x,p.y);

      if(state==0 && points[i]==_profile.getMaxTemperature(_trackingType)) {

        path=fillPath(canvas,path,startPoint,p,0x80ffff00,0x80ff0000);

        startPoint=p;
        state++;
      }
      else if(state==1 && points[i]!=_profile.getMaxTemperature(_trackingType)) {

        path=fillPath(canvas,path,startPoint,p,0x80ff0000,0x80ff0000);

        startPoint=p;
        state++;
      }
      else if(state==2 && i==points.length-1) {

        path=fillPath(canvas,path,startPoint,p,0x80ff0000,0x80ffff00);

        startPoint=p;
        state++;
      }
    }
  }


  /*
   * Fill an area
   */

  protected Path fillPath(Canvas canvas,Path path,Point startPoint,Point p,int start,int end) {

    LinearGradient lg;
    Paint paint;

    // close the path

    path.lineTo(p.x,_origin.y);
    path.lineTo(startPoint.x,_origin.y);
    path.lineTo(startPoint.x,startPoint.y);

    // create gradient

    lg=new LinearGradient(
            startPoint.x,_origin.y,
            p.x,_origin.y,
            start,end,
            Shader.TileMode.CLAMP);

    // create a paint and fill the path

    paint=new Paint(Paint.ANTI_ALIAS_FLAG | Paint.FILTER_BITMAP_FLAG);
    paint.setShader(lg);

    canvas.drawPath(path,paint);

    // create new path for next run

    path=new Path();
    path.moveTo(p.x,p.y);

    return path;
  }


  /*
   * Convert (time,temp) to (x,y)
   */

  public Point reflowPointToChart(int seconds,double p) {

    double xoffset,yoffset,totalTime;

    totalTime=_profile.getAt(_profile.getRegionCount()-1).getEndTime();
    xoffset=(((double)_width-_origin.x)*(double)seconds)/totalTime;
    yoffset=((double)_origin.y*p)/(_profile.getMaxTemperature(_trackingType)+20);

    return new Point(_origin.x+(int)xoffset,_origin.y-(int)yoffset);
  }


  /*
   * Draw a value on the X axis
   */

  protected void drawXValue(double value,Canvas canvas) {

    double offset,totalTime;
    String str;

    totalTime=_profile.getAt(_profile.getRegionCount()-1).getEndTime();
    offset=(((double)_width-_origin.x)*value)/totalTime;
    str=Integer.toString((int)value);

    canvas.drawText(
            str,
            _origin.x+(int)offset,
            _origin.y+AXIS_WIDTH+10+NUMBERS_HEIGHT,
            _numbersPaint);
  }


  /*
   * Draw a value on the Y axis
   */

  protected void drawYValue(double value,Canvas canvas) {

    double offset;
    float textWidth;
    String str;

    offset=((double)_origin.y*value)/(_profile.getMaxTemperature(_trackingType)+20);
    str=Integer.toString((int)value);
    textWidth=_numbersPaint.measureText(str);

    canvas.drawText(
            str,
            _origin.x-AXIS_WIDTH-10-textWidth,
            _origin.y-(int)offset,
            _numbersPaint);
  }


  /*
   * Get/set
   */

  public void setReflowProfile(ReflowProfile profile) { _profile=profile; }
  public void setTrackingType(TrackingType tt) { _trackingType=tt; }
}
