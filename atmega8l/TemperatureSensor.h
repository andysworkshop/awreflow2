/*
 * Andy's Workshop Reflow Controller ATMega328p firmware
 * Copyright (c) 2015 Andy Brown. http://www.andybrown.me.uk
 * Please see website for licensing terms.
 */

#pragma once


namespace awreflow {

  /*
   * SPI temperature sensor
   */

  class TemperatureSensor {

    public:
      enum class Status : uint8_t {
        UNKNOWN    = 0,
        OK         = 1,
        SCV_FAULT  = 2,
        SCG_FAULT  = 3,
        OC_FAULT   = 4
      };

      // simple response structure to combine the status
      // and the value 

      struct Response {
        int16_t celsius;
        Status status;
      };

    protected:
      enum {
        POLL_INTERVAL = 200       // interval between polling the sensor
      };

      uint32_t _lastTemperatureTime;
      TemperatureSensor::Response _lastTemperature;

    protected:
      void processResponse(uint32_t value);

    public:
      TemperatureSensor();

      const Response& getLastTemperature() const;
      bool loop();
  };


  /*
   * Constructor
   */

  inline TemperatureSensor::TemperatureSensor() {

    _lastTemperatureTime=0xFFFFFFFF-10000;
    _lastTemperature.status=TemperatureSensor::Status::UNKNOWN;
    _lastTemperature.celsius=0;
  }


  /*
   * Main loop. Returns true if a new temperature has been received and is ready for processing
   * or false in any other case.
   */

  inline bool TemperatureSensor::loop() {

    uint8_t i;
    uint32_t value;

    // if ready then process the raw response

    if(MillisecondTimer::hasTimedOut(_lastTemperatureTime,POLL_INTERVAL)) {

      // bring CS low

      GpioSpiCs::reset();

      // clock 4 bytes from the SPI bus

      value=0;
      for(i=0;i<4;i++) {
        
        SPDR=0;                             // start "transmitting" (actually just clocking)
        while((SPSR & (1<<SPIF))==0);       // wait until transfer ends
        
        value<<=8;                          // make space for the byte
        value|=SPDR;                        // merge in the new byte
      }

      // restore CS high

      GpioSpiCs::set();       // deselect the slave

      // parse out the value/code from the packed response

      processResponse(value);
      _lastTemperatureTime=MillisecondTimer::millis();
      return true;
    }

    return false;
  }
      

  /*
   * Get the last temperature
   */

  inline const TemperatureSensor::Response& TemperatureSensor::getLastTemperature() const {
    return _lastTemperature;
  }


  /*
   * Read a value. Return the value in the parameter and a
   * status code to indicate fault or OK.
   */

  inline void TemperatureSensor::processResponse(uint32_t value) {

    // we need a sane compiler

    static_assert(sizeof(Response)==3,"Internal compiler fail: sizeof(Response)!=3");

    if(value & 1)
      _lastTemperature.status=Status::OC_FAULT;
    else if(value & 2)
      _lastTemperature.status=Status::SCG_FAULT;
    else if(value & 4)
      _lastTemperature.status=Status::SCV_FAULT;
    else {

      // if negative value, drop the lower 18 bits and extend sign bits

      if(value & 0x80000000)
        value=0xFFFFC000 | ((value >> 18) & 0x00003FFFF);
      else
        value >>= 18;

      // resolution is 0.25C, divide by 4 to get the integer value

      _lastTemperature.celsius=value/4;
      _lastTemperature.status=Status::OK;
    } 
  }
}
