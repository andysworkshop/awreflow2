/*
 * Andy's Workshop Reflow Controller ATMega328p firmware
 * Copyright (c) 2015 Andy Brown. http://www.andybrown.me.uk
 * Please see website for licensing terms.
 */

#pragma once


namespace awreflow {


  /*
   * display the current temperature with a banner at the top
   * to indicate that we are remoting
   */

  struct UiRemotingDisplay : UiBannerComponent, UiTemperatureComponent {

    uint8_t _uiDutyCycle;

    /*
     * String table indexes
     */

    enum {
      NO_BLUETOOTH_INDEX = 0,
      COMMUNICATING_INDEX = 1
    };

    UiRemotingDisplay(TemperatureSensor& temperatureSensor);
    void loop(bool activeCommands);
  };


  /*
   * Constructor
   */

  inline UiRemotingDisplay::UiRemotingDisplay(TemperatureSensor& temperatureSensor)
    : UiBannerComponent(RemotingString),
      UiTemperatureComponent(temperatureSensor),
      _uiDutyCycle(0) {
  }


  /*
   * Main loop
   */


  /*
   * Update the display. This is called once per second during reflow
   */

  inline void UiRemotingDisplay::loop(bool activeCommands) {

    char buffer[15];

    UiTemperatureComponent::loop();

    // format the display: "NO LINK / OVEN xxx%""

    if(activeCommands) {

      buffer[0]='O';
      buffer[1]='V';
      buffer[2]='E';
      buffer[3]='N';
      buffer[4]=' ';

      utils::write3digits(_uiDutyCycle,buffer+5);
      buffer[8]='%';
      buffer[9]='\0';
    }
    else
      strcpy_P(buffer,NoBluetoothString);

    Nokia5110::writeString(0,5,buffer,false);
  }
}
