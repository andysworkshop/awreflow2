/*
 * Andy's Workshop Reflow Controller ATMega328p firmware
 * Copyright (c) 2015 Andy Brown. http://www.andybrown.me.uk
 * Please see website for licensing terms.
 */

#pragma once


namespace awreflow {

  /*
   * A reflow region is the ending temperature and the ending time. The desired progression is a straight
   * line from the current time and temperature to this region's temp/time
   */

  struct ReflowRegion {
    uint16_t endingTime;
    uint16_t endingTemperature;
  };

  extern const ReflowRegion LeadReflowRegions[6];
  extern const ReflowRegion LeadFreeReflowRegions[6];
}
