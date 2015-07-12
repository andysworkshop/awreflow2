/*
 * Andy's Workshop Reflow Oven Controller.
 * Copyright (c) 2014 Andy Brown. All rights reserved.
 * Please see website (http://www.andybrown.me.uk) for full details.
 */


#include "Application.h"


namespace awreflow {


  /*
   * PID class
   */

  class Pid {

    public:
      typedef double pid_variable_t;
    
    protected:

      pid_variable_t _kp;
      pid_variable_t _ki;
      pid_variable_t _kd;

      pid_variable_t _lastError;
      pid_variable_t _integral;

    public:
      void setup(pid_variable_t kp,pid_variable_t ki,pid_variable_t kd);
      uint8_t loop(pid_variable_t desiredTemperature,pid_variable_t currentTemperature);
  };


  /*
   * Setup for a new run
   */

  inline void Pid::setup(pid_variable_t kp,pid_variable_t ki,pid_variable_t kd) {
    _kp=kp;
    _ki=ki;
    _kd=kd;
    _lastError=0,
    _integral=0;
  }


  /*
   * Update the algorithm with the current error and get a percentage value back
   * that can be used as a PWM duty cycle (0..100). This method should be called at
   * a fixed time interval.
   */

  inline uint8_t Pid::loop(pid_variable_t desiredTemperature,pid_variable_t currentTemperature) {

    pid_variable_t error,pwm,derivative;

    // current error term is the difference between desired and current temperature

    error=desiredTemperature-currentTemperature;

    // update the integral (historical error)

    _integral+=error;
    _integral=max(min((pid_variable_t)100,_integral),(pid_variable_t)0);

    // the derivative term

    derivative=error-_lastError;

    // calculate the control variable

    pwm=(_kp*error)+(_ki*_integral)+(_kd*derivative);
    pwm=max(min((pid_variable_t)100,pwm),(pid_variable_t)0);

    // save the last error

    _lastError=error;

    // return the control variable

    return static_cast<uint8_t>(pwm);
  }
}
