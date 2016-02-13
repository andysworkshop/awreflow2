/*
 * Andy's Workshop Reflow Controller ATMega328p firmware
 * Copyright (c) 2015 Andy Brown. http://www.andybrown.me.uk
 * Please see website for licensing terms.
 */

#pragma once


namespace awreflow {


  /*
   * Reflowing progress display. Shows a banner across the top, the current temperature
   * large in the middle and the current time (seconds) and target temperature at the bottom.
   */

  struct UiReflowingDisplay : UiBannerComponent, UiTemperatureComponent {
    
    UiReflowingDisplay(TemperatureSensor& temperatureSensor);
    
    void loop();
    void setStatus(uint16_t currentTime,int16_t desiredTemperature,uint8_t dutyCycle) const;
  };


  /*
   * Constructor
   */

  inline UiReflowingDisplay::UiReflowingDisplay(TemperatureSensor& temperatureSensor)
    : UiBannerComponent(ReflowingString),
      UiTemperatureComponent(temperatureSensor) {
  }


  /*
   * Main loop
   */

  inline void UiReflowingDisplay::loop() {

    // the temperature display needs to be updated

    UiTemperatureComponent::loop();
  }



  /*
   * Update the display. This is called once per second during reflow
   */

  inline void UiReflowingDisplay::setStatus(uint16_t currentTime,int16_t desiredTemperature,uint8_t dutyCycle) const {

    char buffer[15],*ptr;

    memset(buffer,' ',14);

    // format the display: "nnnS mmmC"

    itoa(currentTime,buffer,10);
    for(ptr=buffer;*ptr;ptr++);
    *ptr++='S';
    ptr++;
    itoa(desiredTemperature,ptr,10);
    for(;*ptr;ptr++);
    *ptr++='C';
    ptr++;
    itoa(dutyCycle,ptr,10);
    for(;*ptr;ptr++);
    *ptr++='%';

    buffer[14]='\0';
    
    Nokia5110::writeString(0,5,buffer,false);
  }
}
