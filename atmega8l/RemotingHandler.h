/*
 * Andy's Workshop Reflow Controller ATMega328p firmware
 * Copyright (c) 2015 Andy Brown. http://www.andybrown.me.uk
 * Please see website for licensing terms.
 */

#pragma once


namespace awreflow {

  /*
   * Handler for the remoting state
   */

  class RemotingHandler {

    protected:
      Bluetooth& _bluetooth;
      ActionButton& _actionButton;
      OvenControl& _oven;

      UiRemotingDisplay _display;
      uint32_t _lastCommandTime;

    protected:
      void processNextCommand();
      void readSettings() const;
      bool setDutyCycleCommand(uint8_t dutyCycle) const;
      bool setOvenZero(uint8_t zero) const;
      bool setLcdBacklight(uint8_t backlight) const;
      bool setLcdContrast(uint8_t contrast) const;

    public:
      RemotingHandler(TemperatureSensor& temperatureSensor,Bluetooth& bluetooth,ActionButton& actionButton,OvenControl& oven);
      ~RemotingHandler();

      ProgramState loop();
  };


  /*
   * Constructor
   */

  inline RemotingHandler::RemotingHandler(TemperatureSensor& temperatureSensor,Bluetooth& bluetooth,ActionButton& actionButton,OvenControl& oven)
    : _bluetooth(bluetooth),
      _actionButton(actionButton),
      _oven(oven),
      _display(temperatureSensor) {

    GpioPairedStatus::reset();
    _lastCommandTime=0xFFFFFFFF-10000;     // ensure that we don't power up with the light on
  }


  /*
   * Destructor
   */

  inline RemotingHandler::~RemotingHandler() {

    // turn off the oven and the active light

    _oven.setDutyCycle(0,false);
    GpioPairedStatus::reset();
  }


  /*
   * Main Loop
  */

  inline ProgramState RemotingHandler::loop() {

    bool activeCommands=false;

    do {

      // main loops

      _display.loop(activeCommands);
      _actionButton.loop();

      // read command and process

      processNextCommand();

      // if we had a command in the last 5 seconds then the light stays on

      activeCommands=!MillisecondTimer::hasTimedOut(_lastCommandTime,5000);
      GpioPairedStatus::changeTo(activeCommands);

      // if the button is pressed while no command is active then move on

    } while(activeCommands || !_actionButton.isPressed());

    // button has been pressed, now selecting mode

    return ProgramState::SELECTING_MODE;
  }


  /*
   * Process the next command if one is available
   */

  inline void RemotingHandler::processNextCommand() {

    Command c;
    uint8_t parameter;
    bool ok;

    // read command, return if not ready

    if((c=_bluetooth.getCommandReceived())==Command::NONE)
      return;

    // get the optional parameter

    parameter=_bluetooth.getSingleByteParameter();

    // just had a command

    _lastCommandTime=MillisecondTimer::millis();

    // which command?

    switch(c) {

      case Command::READ_SETTINGS:
        readSettings();
        return;

      // all the following commands return the temperature

      case Command::READ_TEMPERATURE:       
        ok=true;
        break;

      case Command::SET_DUTY_CYCLE:
        ok=setDutyCycleCommand(parameter);
        break;

      case Command::SET_OVEN_ZERO:
        ok=setOvenZero(parameter);
        break;

      case Command::SET_LCD_BACKLIGHT:
        ok=setLcdBacklight(parameter);
        break;

      case Command::SET_LCD_CONTRAST:
        ok=setLcdContrast(parameter);
        break;

      default:        // unknown/corrupted command: NACK it
        _bluetooth.writeReply(Acknowledge::NACK,nullptr,0);
        return;
    }

    if(ok) {

      // write the temperature back as a response
      
      const TemperatureSensor::Response& lastTemperature(_display.getTemperatureSensor().getLastTemperature());
      _bluetooth.writeReply(Acknowledge::ACK,&lastTemperature,sizeof(lastTemperature));
    }
    else
      _bluetooth.writeReply(Acknowledge::NACK,nullptr,0);
  }


  /*
   * Read and set a new duty cycle
   */

  inline bool RemotingHandler::setDutyCycleCommand(uint8_t dutyCycle) const {

    // ACK if in range otherwise NACK 

    if(dutyCycle>100)
      return false;
    else {
      _oven.setDutyCycle(dutyCycle,false);
      return true;
    }
  }


  /*
   * Store the oven zero calibration setting
   */

  inline bool RemotingHandler::setOvenZero(uint8_t zero) const {

    if(zero>100)
      return false;
    else {

      // store the value

      Eeprom::Writer::zeroPercentage(zero);
      return true;
    }
  }


  /*
   * Store the LCD backlight setting
   */

  inline bool RemotingHandler::setLcdBacklight(uint8_t backlight) const {

    if(backlight>100)
      return false;
    else {

      // store the value

      Nokia5110::setBacklight(backlight);
      Eeprom::Writer::lcdBacklight(backlight);
      
      return true;
    }
  }


  /*
   * Store the LCD contrast setting
   */

  inline bool RemotingHandler::setLcdContrast(uint8_t contrast) const {

    if(contrast>127)
      return false;
    else {

      // store the value

      Nokia5110::setContrast(contrast);
      Eeprom::Writer::lcdContrast(contrast);
      
      return true;
    }
  }


  /*
   * Read the general settings (oven zero, lcd backlight, contrast)
   */

  inline void RemotingHandler::readSettings() const {

    uint8_t response[3];

    // it's a 3-byte response with the requested codes

    response[0]=Eeprom::Reader::zeroPercentage();
    response[1]=Eeprom::Reader::lcdBacklight();
    response[2]=Eeprom::Reader::lcdContrast();

    _bluetooth.writeReply(Acknowledge::ACK,response,sizeof(response));
  }
}
