/*
 * Andy's Workshop Reflow Controller ATMega328p firmware
 * Copyright (c) 2015 Andy Brown. http://www.andybrown.me.uk
 * Please see website for licensing terms.
 */

#include "Application.h"


namespace awreflow {

  /*
   * Constant strings
   */

  const char RemotingString[] PROGMEM = "  LISTENING   ";
  const char SelectingProfileString[] PROGMEM = "   PROFILE    ";
  const char SelectingModeString[] PROGMEM = "   OPTIONS    ";
  const char SelectingProportionalString[] PROGMEM = " PROPORTIONAL ";
  const char SelectingIntegerString[] PROGMEM = "   INTEGER    ";
  const char SelectingDerivativeString[] PROGMEM = "  DERIVATIVE  ";
  const char ReflowReadyString[] PROGMEM = " REFLOW READY ";
  const char CalibratingString[] PROGMEM = "  TEMP OFFSET ";
  const char ReflowingString[] PROGMEM = "  REFLOWING   ";
  const char LcdOptionsString[] PROGMEM = " LCD OPTIONS  ";
  const char LcdBacklightString[] PROGMEM = "  BACKLIGHT   ";
  const char LcdContrastString[] PROGMEM = "   CONTRAST   ";

  /*
   * Constant string arrays for the mode selection
   */

  const char SelectingModeString1[] PROGMEM = "REFLOW";
  const char SelectingModeString2[] PROGMEM = "PID";
  const char SelectingModeString3[] PROGMEM = "TEMP OFFSET";
  const char SelectingModeString4[] PROGMEM = "LCD";

  PGM_P const SelectingModeStringTable[4] PROGMEM = {
    SelectingModeString1,
    SelectingModeString2,
    SelectingModeString3,
    SelectingModeString4
  };


  /*
   * Constant string arrays for the profile selection
   */

  const char SelectingProfileString1[] PROGMEM = "LEADED";
  const char SelectingProfileString2[] PROGMEM = "LEAD FREE";

  PGM_P const SelectingProfileStringTable[2] PROGMEM = {
    SelectingProfileString1,
    SelectingProfileString2
  };


  /*
   * Constant string arrays for the reflow ready selection
   */

  const char ReflowReadyString1[] PROGMEM = "REFLOW NOW";
  const char ReflowReadyString2[] PROGMEM = "CANCEL";

  PGM_P const ReflowReadyStringTable[2] PROGMEM = {
    ReflowReadyString1,
    ReflowReadyString2
  };


  /*
   * Constant string arrays for the remoting status
   */

  const char RemotingStatus1[] PROGMEM = "NO BLUETOOTH ";
  const char RemotingStatus2[] PROGMEM = "COMMUNICATING";

  PGM_P const RemotingStatusStringTable[2] PROGMEM = {
    RemotingStatus1,
    RemotingStatus2
  };


  /*
   * Constant strings for the LCD options
   */

  const char LcdOptions1[] PROGMEM = "BACKLIGHT";
  const char LcdOptions2[] PROGMEM = "CONTRAST";

  PGM_P const LcdOptionsStringTable[2] PROGMEM = {
    LcdOptions1,
    LcdOptions2
  };
}
