/*
 * Andy's Workshop Reflow Controller ATMega328p firmware
 * Copyright (c) 2015 Andy Brown. http://www.andybrown.me.uk
 * Please see website for licensing terms.
 */

#include "Application.h"


namespace awreflow {

  /*
   * Reflow regions for the leaded profile
   */

  const ReflowRegion LeadReflowRegions[6] PROGMEM = {
    { 90,150  },      // preheat
    { 180,180 },      // soak
    { 200,210 },      // ramp up
    { 220,210 },      // reflow
    { 240,180 },      // ramp down
    { 270,25  }       // cool down
  };


  /*
   * Reflow regions for the lead-free profile
   */

  const ReflowRegion LeadFreeReflowRegions[6] PROGMEM = {
    { 90,150  },
    { 180,217 },
    { 200,240 },
    { 220,240 },
    { 240,217 },
    { 270,25  },
  };
}
