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

  struct UiRemotingDisplay : UiBannerComponent, UiTemperatureComponent, UiStatusComponent {

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
      UiStatusComponent(RemotingStatusStringTable) {
  }


  /*
   * Main loop
   */

  inline void UiRemotingDisplay::loop(bool activeCommands) {

    // the temperature display needs to be updated

    UiTemperatureComponent::loop();

    // update the status display

    UiStatusComponent::loop(activeCommands ? COMMUNICATING_INDEX : NO_BLUETOOTH_INDEX);
  }
}
