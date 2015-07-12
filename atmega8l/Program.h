/*
 * Andy's Workshop Reflow Controller ATMega328p firmware
 * Copyright (c) 2015 Andy Brown. http://www.andybrown.me.uk
 * Please see website for licensing terms.
 */


#pragma once


namespace awreflow {

  /*
   * Main program class
   */

  class Program {

    protected:
      TemperatureSensor _temperatureSensor;
      OvenControl _oven;
      Bluetooth _bluetooth;
      Encoder _encoder;
      ActionButton _actionButton;

    public:
      Program();

      void run();
      void test();
  };
}
