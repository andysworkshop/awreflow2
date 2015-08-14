/*
 * Andy's Workshop Reflow Controller ATMega328p firmware
 * Copyright (c) 2015 Andy Brown. http://www.andybrown.me.uk
 * Please see website for licensing terms.
 */

#pragma once


namespace awreflow {

  /*
   * Handler for the reflowing state
   */

  class ReflowingHandler {

    protected:

      enum class ActionTaken : uint8_t {
        NOTHING,
        UPDATED,
        STOP
      };

      ActionButton& _actionButton;
      OvenControl& _oven;      

      ReflowProfile _profile;
      
      Pid _pid;
      int16_t _desiredTemperature;
      int16_t _temperatureStep;

      uint16_t _currentTime;
      uint16_t _currentRegion;
      uint32_t _lastTick;
      
      bool paused;
      UiReflowingDisplay _display;

    protected:
      ActionTaken reflow();
      void calcTemperatureStep();

    public:
      ReflowingHandler(TemperatureSensor& temperatureSensor,ActionButton& actionButton,OvenControl& oven);
      ~ReflowingHandler();

      ProgramState loop();
  };


  /*
   * Constructor
   */

  inline ReflowingHandler::ReflowingHandler(TemperatureSensor& temperatureSensor,ActionButton& actionButton,OvenControl& oven)
    : _actionButton(actionButton),
      _oven(oven),
      _display(temperatureSensor) {

    uint16_t p,i,d;

    // set up the PID class

    p=Eeprom::Reader::Reader::constant(Eeprom::Location::PROPORTIONAL);
    i=Eeprom::Reader::Reader::constant(Eeprom::Location::INTEGER);
    d=Eeprom::Reader::Reader::constant(Eeprom::Location::DERIVATIVE);

    _pid.setup(p,i,d);

    // get the appropriate profile

    if(static_cast<ReflowProfileName>(Eeprom::Reader::readByte(Eeprom::Location::PROFILE))==ReflowProfileName::LEADED)
      _profile=ReflowProfile::leaded();
    else
      _profile=ReflowProfile::leadFree();

    _desiredTemperature=25;       // all profiles start at 25
    _currentTime=0;
    _currentRegion=0;
    _lastTick=MillisecondTimer::millis();

    calcTemperatureStep();
  }


  /*
   * Destructor
   */

  inline ReflowingHandler::~ReflowingHandler() {

    // turn off the oven

    _oven.setDutyCycle(0);
  }


  /*
   * Main Loop
  */

  inline ProgramState ReflowingHandler::loop() {

    do {

      // main loops

      _display.loop();
      _actionButton.loop();

    } while(reflow()!=ActionTaken::STOP && !_actionButton.isPressed());

    // back to the start
    
    return ProgramState::REMOTING;
  }


  /*
   * Program logic for the reflow. Return true if we updated during this call or false
   * if it was too soon (we update once per second).
   */

  inline ReflowingHandler::ActionTaken ReflowingHandler::reflow() {

    uint8_t dutyCycle;

    // get the current sensor state

    const TemperatureSensor::Response& lastTemperature(_display.getTemperatureSensor().getLastTemperature());

    // do nothing if we're back before a second has elapsed or we're too early and the first
    // temperature reading as not yet been taken

    if(!MillisecondTimer::hasTimedOut(_lastTick,1000) || lastTemperature.status==TemperatureSensor::Status::UNKNOWN)
      return ActionTaken::NOTHING;

    // if there's been a hardware failure in the temperature sensor then bail out

    if(lastTemperature.status!=TemperatureSensor::Status::OK)
      return ActionTaken::STOP;

    // reset for the next update (now, not after we've burned some cycles doing our update)

    _lastTick=MillisecondTimer::millis();

    // if we're in the first second and we are below the starting temperature then auto-pause
    // until we've heated up to the starting temperature (25 degrees)

    if(_currentTime || lastTemperature.celsius>=_desiredTemperature) {

      // get the current region out of PROGMEM

      ReflowRegion region=_profile.getRegion(_currentRegion);

      // update seconds and see if we've hit the end

      if(region.endingTime==_currentTime) {

        // we have hit the ending time of the current region. if this is the last region
        // then the whole process has completed

        _currentRegion++;
        if(_currentRegion==_profile.regionCount)
          return ActionTaken::STOP;

        // we're in a new region. set up the parameters for this leg.

        calcTemperatureStep();
      }

      // update time and desired temperature

      _currentTime++;
      _desiredTemperature+=_temperatureStep;
    }

    // run the PID algorithm and set the relay PWM value from the output

    dutyCycle=_pid.loop(_desiredTemperature,lastTemperature.celsius);
    _oven.setDutyCycle(dutyCycle);

    // update the display

    _display.setStatus(_currentTime,_desiredTemperature,dutyCycle);

    // continue

    return ActionTaken::UPDATED;
  }


  /*
   * Calculate the temperature step value
   */

  inline void ReflowingHandler::calcTemperatureStep() {

    ReflowRegion region=_profile.getRegion(_currentRegion);

    _temperatureStep=(region.endingTemperature-_desiredTemperature)/(region.endingTime-_currentTime);    
  }
}
