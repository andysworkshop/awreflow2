/*
 * Andy's Workshop Reflow Controller ATMega328p firmware
 * Copyright (c) 2015 Andy Brown. http://www.andybrown.me.uk
 * Please see website for licensing terms.
 */

#pragma once


namespace awreflow {

  /*
   * Standard template implementations of min and max
   */
   
  template<class T> 
  const T& max(const T& a, const T& b) {
    return (a>b) ? a : b;
  }


  template<class T> 
  const T& min(const T& a, const T& b) {
    return (a<b) ? a : b;
  }
}
