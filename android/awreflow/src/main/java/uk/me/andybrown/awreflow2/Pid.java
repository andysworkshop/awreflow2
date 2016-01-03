// Andy's Workshop Open Source Reflow Controller
// Copyright (c) 2015 Andy Brown. All rights Reserved.
// Please see website (http://www.andybrown.me.uk) for full license details.

package uk.me.andybrown.awreflow2;


/*
 * PID algorithm
 */

public class Pid {

  /*
   * Members
   */

  final protected double _kp;
  final protected double _ki;
  final protected double _kd;

  protected double _lastError;
  protected double _integral;


  /*
   * Constructor
   */

  public Pid(double kp,double ki,double kd) {

    _kp=kp;
    _ki=ki;
    _kd=kd;

    _lastError=0;
    _integral=0;
  }


  /*
   * Update the algorithm with the current error and get a percentage value back
   * that can be used as a PWM duty cycle (0..100). This method should be called at
   * a fixed time interval.
   */

   public byte update(double desiredTemperature,double currentTemperature) {

    double error,pwm,derivative;

    // current error term is the difference between desired and current temperature

    error=desiredTemperature-currentTemperature;

    // update and cap the integral (historical error)

    _integral+=error;
    _integral=Math.max(Math.min(100.0,_integral),0.0);

    // the derivative term

    derivative=error-_lastError;

    // calculate the control variable

    pwm=(_kp*error)+(_ki*_integral)+(_kd*derivative);
    pwm=Math.max(Math.min(100.0,pwm),0.0);

    // save the last error

    _lastError=error;

    // return the control variable

    return (byte)pwm;
  }
}
