/*
 * Andy's Workshop Reflow Controller ATMega328p firmware
 * Copyright (c) 2015 Andy Brown. http://www.andybrown.me.uk
 * Please see website for licensing terms.
 */

#pragma once


namespace awreflow {
  namespace utils {

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


    /*
     * Write 3 digits to a buffer, right justified with spaces
     */

    inline void write3digits(uint16_t number,char *buffer) {

      char c;

      c=number/100;
      buffer[0]=c ? c+'0' : ' ';
      number%=100;

      c=number/10; 
      buffer[1]=c || buffer[0]!=' ' ? c+'0' : ' ';
      number%=10;

      buffer[2]=number+'0';
    }
  }
}
