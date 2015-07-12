/*
 * Andy's Workshop Reflow Controller ATMega328p firmware
 * Copyright (c) 2015 Andy Brown. http://www.andybrown.me.uk
 * Please see website for licensing terms.
 */

#pragma once


namespace awreflow {


  /*
   * Base class for states that display the current temperature
   * on the LED display
   */

  class TemperatureDisplay {

    protected:
      TemperatureSensor &_temperatureSensor;
      Segments& _display;

    protected:
      TemperatureDisplay(TemperatureSensor& temperatureSensor,Segments& segments);

      void loop();      
  };


  /*
   * Constructor
   */

  inline TemperatureDisplay::TemperatureDisplay(TemperatureSensor& temperatureSensor,Segments& display)
    : _temperatureSensor(temperatureSensor),
      _display(display) {
  }


  /*
   * Read and display the temperature every second
   */

  inline void TemperatureDisplay::loop() {

    // keep the updates happening

    _temperatureSensor.loop();

    // get the last reading

    const TemperatureSensor::Response& lastTemperature(_temperatureSensor.getLastTemperature());

    // display it

    switch(lastTemperature.status) {

      case TemperatureSensor::Status::OK:
        _display.setCharacters(lastTemperature.celsius);
        break;

      case TemperatureSensor::Status::SCV_FAULT:
        _display.setCharacters("E 1 ");
        break;
      
      case TemperatureSensor::Status::SCG_FAULT:
        _display.setCharacters("E 2 ");
        break;

      case TemperatureSensor::Status::OC_FAULT:
        _display.setCharacters("E 3 ");
        break;

      default:
        break;
    }
  }
}
