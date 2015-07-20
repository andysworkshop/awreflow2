/*
 * Andy's Workshop Reflow Controller ATMega328p firmware
 * Copyright (c) 2015 Andy Brown. http://www.andybrown.me.uk
 * Please see website for licensing terms.
 */

#pragma once


namespace awreflow {


  /*
   * Display the current temperature in the center of the display
   */

  class UiTemperatureComponent {

    protected:
      TemperatureSensor& _temperatureSensor;

    protected:
      void show(const char *buffer) const;

    public:
      UiTemperatureComponent(TemperatureSensor& temperatureSensor);

      void loop();

      TemperatureSensor& getTemperatureSensor();
  };


  /*
   * Constructor
   */

  inline UiTemperatureComponent::UiTemperatureComponent(TemperatureSensor& temperatureSensor)
    : _temperatureSensor(temperatureSensor) {
  }


  /*
   * Main loop
   */

  inline void UiTemperatureComponent::loop() {

    // loop the temperature sensor and check for a new value to display

    if(!_temperatureSensor.loop())
      return; 

    // get the last reading. we have 200ms to process it before the sensor takes the SPI bus

    const TemperatureSensor::Response& lastTemperature(_temperatureSensor.getLastTemperature());

    // display it

    switch(lastTemperature.status) {

      case TemperatureSensor::Status::OK:
        Nokia5110::writeLargeTemperature(0,2,lastTemperature.celsius);
        break;

      case TemperatureSensor::Status::SCV_FAULT:
        show("E 1 ");
        break;
      
      case TemperatureSensor::Status::SCG_FAULT:
        show("E 2 ");
        break;

      case TemperatureSensor::Status::OC_FAULT:
        show("E 3 ");
        break;

      default:
        break;
    }
  }


  /*
   * Show the string on the third line and fill to the end
   */

  inline void UiTemperatureComponent::show(const char *buffer) const {
    
    const char *ptr;
    uint8_t count;

    Nokia5110::moveTo(0,2);

    count=0;
    ptr=buffer;

    while(*ptr) {
      Nokia5110::writeCharacter(*ptr++,false);
      count++;
    }

    for(;count<12;count++)
      Nokia5110::writeCharacter(' ',false);
  }


  /*
   * Get a reference to the temperature sensor
   */
   
  inline TemperatureSensor& UiTemperatureComponent::getTemperatureSensor() {
    return _temperatureSensor;
  }
}
