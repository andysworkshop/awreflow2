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
  const char CalibratingString[] PROGMEM = "ZERO CALIBRATE";
  const char ReflowingString[] PROGMEM = "  REFLOWING   ";
  const char LcdOptionsString[] PROGMEM = " LCD OPTIONS  ";
  const char LcdBacklightString[] PROGMEM = "  BACKLIGHT   ";
  const char LcdContrastString[] PROGMEM = "   CONTRAST   ";

  /*
   * Constant string arrays for the mode selection
   */

  const char SelectingModeString1[] PROGMEM = "Reflow";
  const char SelectingModeString2[] PROGMEM = "PID";
  const char SelectingModeString3[] PROGMEM = "Calibrate oven";
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

  const char SelectingProfileString1[] PROGMEM = "Leaded";
  const char SelectingProfileString2[] PROGMEM = "Lead free";

  PGM_P const SelectingProfileStringTable[2] PROGMEM = {
    SelectingProfileString1,
    SelectingProfileString2
  };


  /*
   * Constant string arrays for the reflow ready selection
   */

  const char ReflowReadyString1[] PROGMEM = "Reflow now";
  const char ReflowReadyString2[] PROGMEM = "Cancel";

  PGM_P const ReflowReadyStringTable[2] PROGMEM = {
    ReflowReadyString1,
    ReflowReadyString2
  };


  /*
   * Constant string arrays for the remoting status
   */

  const char RemotingStatus1[] PROGMEM = "No bluetooth ";
  const char RemotingStatus2[] PROGMEM = "Communicating";

  PGM_P const RemotingStatusStringTable[2] PROGMEM = {
    RemotingStatus1,
    RemotingStatus2
  };


  /*
   * Constant strings for the LCD options
   */

  const char LcdOptions1[] PROGMEM = "Backlight";
  const char LcdOptions2[] PROGMEM = "Contrast";

  PGM_P const LcdOptionsStringTable[2] PROGMEM = {
    LcdOptions1,
    LcdOptions2
  };
}
