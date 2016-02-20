/*
 * Andy's Workshop Reflow Controller ATMega328p firmware
 * Copyright (c) 2015 Andy Brown. http://www.andybrown.me.uk
 * Please see website for licensing terms.
 */

#pragma once


namespace awreflow {

  /*
   * Strings stored in flash
   */

  extern const char RemotingString[];
  extern const char NoBluetoothString[];
  extern const char SelectingProfileString[];
  extern const char SelectingProportionalString[];
  extern const char SelectingIntegerString[];
  extern const char SelectingDerivativeString[];
  extern const char SelectingModeString[];
  extern const char ReflowReadyString[];
  extern const char CalibratingString[];
  extern const char ReflowingString[];
  extern const char LcdOptionsString[];
  extern const char LcdBacklightString[];
  extern const char LcdContrastString[];

  
  /*
   * String tables stored in flash
   */

  extern PGM_P const SelectingModeStringTable[4];
  extern PGM_P const SelectingProfileStringTable[2];
  extern PGM_P const ReflowReadyStringTable[2];
  extern PGM_P const LcdOptionsStringTable[2];
}
