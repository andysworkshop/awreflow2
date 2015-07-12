/*
 * Andy's Workshop Reflow Controller ATMega328p firmware
 * Copyright (c) 2015 Andy Brown. http://www.andybrown.me.uk
 * Please see website for licensing terms.
 */

#include "Application.h"


// back pointer to the class where we'll actually handle the interrupt

namespace awreflow {
  OvenControl *OvenControlInstance;
}


/*
 * Zero crossing interrupt handler
 */

ISR(INT0_vect) {
  awreflow::OvenControlInstance->zeroCrossingHandler();
}


/*
 * Timer1 comparison value A matched
 */

ISR(TIMER2_COMP_vect) {
  awreflow::OvenControlInstance->timerComparatorHandler();
}


/*
 * Timer1 overflow handler
 */

ISR(TIMER2_OVF_vect) {
  awreflow::OvenControlInstance->timerOverflowHandler();
}
