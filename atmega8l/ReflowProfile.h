/*
 * Andy's Workshop Reflow Controller ATMega328p firmware
 * Copyright (c) 2015 Andy Brown. http://www.andybrown.me.uk
 * Please see website for licensing terms.
 */

#pragma once


namespace awreflow {

  /*
   * A reflow profile is the number of regions in the profile and a PROGMEM
   * pointer to the structures
   */

  struct ReflowProfile {

    uint8_t regionCount;
    const ReflowRegion *regions;        // PROGMEM pointer to the regions


    /*
     * Copy out a region from PROGMEM
     */

    ReflowRegion getRegion(uint8_t index) {

      ReflowRegion r;

      memcpy_P(&r,&regions[index],sizeof(ReflowRegion));
      return r;
    }


    /*
     * Get a leaded profile
     */

    static ReflowProfile leaded() {
      return ReflowProfile {
        sizeof(LeadReflowRegions)/sizeof(LeadReflowRegions)[0],
        LeadReflowRegions
      };
    }

    /*
     * Get a lead free profile
     */

    static ReflowProfile leadFree() {
      return ReflowProfile {
        sizeof(LeadFreeReflowRegions)/sizeof(LeadFreeReflowRegions)[0],
        LeadFreeReflowRegions
      };
    }
  };
}
