#include "Application.h"


/*
 * Main entry point
 */

int main() {

  // interrupts off

  cli();

  // verify the EEPROM state

  awreflow::Eeprom::verifyState();

  // declare the program class

  awreflow::Program p;
  p.run(); 
 
  // not reached

  return 0;
}


namespace awreflow {

  /*
   * Constructor: all GPIO pins are initialised here
   */

  Program::Program() {

    // SPI limiting values:
    //   1. Max Nokia 5110 LCD clock = 4MHz
    //   2. Max MAX31855 clock = 5MHz
    // Selected clock is osc/2, mode is 0

    SPCR=(1 << MSTR) |                  // master
         (1 << SPE) |                   // enabled
         (0 << SPR1) | (0 << SPR0);     // (for clarity) we are intentionally choosing fosc/4 here

    SPSR |= (1 << SPI2X);

    // clear interrupt flag by reading this register

    uint8_t dummy __attribute__((unused))=SPSR;
    dummy=SPDR;

    // bluetooth pins

    GpioUartTx::setup();
    GpioUartRx::setup();
    GpioUartRx::set();    // pullup

    // display pins

    GpioScreenDataCommand::setup();
    GpioScreenLed::setup();
    GpioScreenReset::setup();
    GpioScreenSelect::setup();
    GpioScreenSelect::set();

    // setup the SPI pins

    GpioSpiCs::setup();
    GpioSpiCs::set();
    
    GpioSpiMiso::setup();
    GpioSpiMosi::setup();
    GpioSpiClk::setup();

    // zero crossing sense pin

    GpioZeroSense::setup(); 

    // oven activation pin

    GpioActivateOven::setup();
    GpioActivateOven::reset();

    //  encoder pins with pullups

    GpioEncoderA::setup();
    GpioEncoderB::setup();

    GpioEncoderA::set();
    GpioEncoderB::set();

    // action button with pullup

    GpioActionButton::setup();
    GpioActionButton::set();

    // paired status

    GpioPairedStatus::setup();
    GpioPairedStatus::reset(); 

    // initialisers

    MillisecondTimer::setup();
    Nokia5110::setup();
  }


  /*
   * Test stuff: delete before launch
   */

  void Program::test() {
  }


  /*
   * Run this program and don't return
   */

  void Program::run() {

    ProgramState state;

    // enable global interrupt

    sei();

    // upgrade the HC-06 baud rate to 38400

    _bluetooth.upgradeBaudRate();
    
    // set the backlight

    Nokia5110::setBacklight(Eeprom::Reader::lcdBacklight());

    // first state is remoting

    state=ProgramState::REMOTING;

    // start looping through our state machine. classes handle each state.

    for(;;) {

      // clear down the display and switch the backlight on at 50%

      Nokia5110::clear();

      // move to the new state
      
      switch(state) {

        // power-up state: receiving commands from bluetooth and displaying temperature

        case ProgramState::REMOTING:
          {
            RemotingHandler handler(_temperatureSensor,_bluetooth,_actionButton,_oven);
            state=handler.loop();
          }
          break;

        // select the mode: reflow, pid or calibrate

        case ProgramState::SELECTING_MODE:
          {
            SelectingModeHandler handler(_encoder,_actionButton);
            state=handler.loop();
          }
          break;

        // LCD options: backlight, contrast

        case ProgramState::LCD_OPTIONS:
          {
            LcdOptionsHandler handler(_encoder,_actionButton);
            state=handler.loop();
          }
          break;

        // LCD backlight handler

        case ProgramState::LCD_BACKLIGHT:
          {
            LcdSetOptionHandler handler(_encoder,_actionButton,100,Eeprom::Location::LCD_BACKLIGHT,LcdBacklightString);
            state=handler.loop();
          }
          break;

        // LCD contrast handler

        case ProgramState::LCD_CONTRAST:
          {
            LcdSetOptionHandler handler(_encoder,_actionButton,127,Eeprom::Location::LCD_CONTRAST,LcdContrastString);
            state=handler.loop();
          }
          break;

        // use the encoder to select where zero is on your oven. zero is the point just before
        // you actually get light out of the halogen lamp. Typically 20-30%.

        case ProgramState::CALIBRATING:
          {
            CalibratingHandler handler(_encoder,_actionButton,_oven);
            state=handler.loop();
          }
          break;

        // choose a profile, leaded or unleaded. 5 second inaction timeout takes you
        // back to the remoting state. the chosen value is saved to EEPROM

        case ProgramState::SELECTING_PROFILE:
          {
            SelectingProfileHandler handler(_encoder,_actionButton);
            state=handler.loop();
          }
          break;

        // choose a constant for the proportional value. 5 second inaction timeout takes you
        // back to the remoting state. The chosen value is saved to EEPROM

        case ProgramState::SELECTING_PROPORTIONAL:
          {
            SelectingConstantHandler handler(_encoder,_actionButton,{ Eeprom::Location::PROPORTIONAL,ProgramState::SELECTING_INTEGER, SelectingProportionalString });
            state=handler.loop();
          }
          break;

        // choose a constant for the integer value. 5 second inaction timeout takes you
        // back to the remoting state. The chosen value is saved to EEPROM

        case ProgramState::SELECTING_INTEGER:
          {
            SelectingConstantHandler handler(_encoder,_actionButton,{ Eeprom::Location::INTEGER,ProgramState::SELECTING_DERIVATIVE, SelectingIntegerString });
            state=handler.loop();
          }
          break;

        // choose a constant for the derivative value. 5 second inaction timeout takes you
        // back to the remoting state. The chosen value is saved to EEPROM

        case ProgramState::SELECTING_DERIVATIVE:
          {
            SelectingConstantHandler handler(_encoder,_actionButton,{ Eeprom::Location::DERIVATIVE,ProgramState::REMOTING, SelectingDerivativeString });
            state=handler.loop();
          }
          break;

        // toggle between ready and abort to start/abort the process

        case ProgramState::REFLOW_READY:
          {
            ReflowReadyHandler handler(_encoder,_actionButton);
            state=handler.loop();
          }
          break;

        // ready for action. click the button to start or wait 5 seconds for a timeout. click
        // the button while reflowing to cancel and go back to remoting

        case ProgramState::REFLOWING:
          {
            ReflowingHandler handler(_temperatureSensor,_actionButton,_oven);
            state=handler.loop();
          }
          break; 
      }
    }
  }
}
