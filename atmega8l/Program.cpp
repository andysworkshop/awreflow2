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
