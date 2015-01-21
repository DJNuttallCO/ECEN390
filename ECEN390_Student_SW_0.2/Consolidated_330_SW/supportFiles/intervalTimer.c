/*
 * intervalTimer.c
 *
 *  Created on: Apr 2, 2014
 *      Author: hutch
 */
#include <stdio.h>
#include "intervalTimer.h"
#include "xparameters.h"
#include "xil_io.h"

// Returns the base address of the timer based upon xparameters.h
u32 intervalTimer_getTimerBaseAddress(u32 timerNumber) {
  u32 timerBaseAddress;
  switch (timerNumber) {
  case 0:
	timerBaseAddress = XPAR_AXI_TIMER_0_BASEADDR;
	break;
  case 1:
	timerBaseAddress = XPAR_AXI_TIMER_1_BASEADDR;
	break;
  case 2:
	timerBaseAddress = XPAR_AXI_TIMER_2_BASEADDR;
	break;
  default:
	printf("intervalTimer_getTimerBaseAddress: timerNumber (%lu) is not between 0 and %d.\n\r",
			timerNumber,
			INTERVAL_TIMER_MAX_TIMER_INDEX);
	timerBaseAddress = 0;
 }
  return timerBaseAddress;
}

// Takes a timer number, register offset, and value and writes value to register.
u32 intervalTimer_writeRegister(u32 timerNumber, u32 registerOffset, u32 value) {
  u32 timerBaseAddress = intervalTimer_getTimerBaseAddress(timerNumber);
  Xil_Out32((timerBaseAddress+registerOffset), value);
  return 0;
}

// Takes a timer number, register offset and returns the value stored in the register.
u32 intervalTimer_readRegister(u32 timerNumber, u32 registerOffset) {
  u32 timerBaseAddress = intervalTimer_getTimerBaseAddress(timerNumber);
  return Xil_In32((timerBaseAddress+registerOffset));
}

// Enables all counters. For cascade mode, enable both counters.
u32 intervalTimer_start(u32 timerNumber){
  intervalTimer_writeRegister(timerNumber, INTERVAL_TIMER_TCSRO_REG_OFFSET, INTERVAL_TIMER_64_BIT_COUNTER_START_COMMAND);
  return 0;
}

// Stop the counter 0 (this assumes that we are in CASCADE mode (64-bit).
// In cascade mode, you only need to enable ENT0 bit in the TSCRO to start the 64-bit counter.
u32 intervalTimer_stop(u32 timerNumber) {
//  u32 registerValue = intervalTimer_readRegister(timerNumber, INTERVAL_TIMER_TCSRO_REG_OFFSET);
//  registerValue &= ~INTERVAL_TIMER_TSCR0_ENT0_BIT_MASK;
//  intervalTimer_writeRegister(timerNumber, INTERVAL_TIMER_TCSRO_REG_OFFSET, registerValue);
	intervalTimer_writeRegister(timerNumber, INTERVAL_TIMER_TCSRO_REG_OFFSET, INTERVAL_TIMER_64_BIT_COUNTER_STOP_COMMAND);
  return 0;
}

// Makes sure the timer is off and then reset both 32-bit counters by loading a 0 into them.
u32 intervalTimer_reset(u32 timerNumber) {
  // Load 0 in to the counter 0 load register.
  intervalTimer_writeRegister(timerNumber, INTERVAL_TIMER_TLR0_REG_OFFSET, 0);
  // Load 0 in to the counter 1 load register.
  intervalTimer_writeRegister(timerNumber, INTERVAL_TIMER_TLR1_REG_OFFSET, 0);
  // Load the contents of load register 0 into counter 0.
  intervalTimer_writeRegister(timerNumber, INTERVAL_TIMER_TCSRO_REG_OFFSET, INTERVAL_TIMER_TSCR0_LOAD0_BIT_MASK);
  // Load the contents of load register 1 into counter 1.
  intervalTimer_writeRegister(timerNumber, INTERVAL_TIMER_TCSR1_REG_OFFSET, INTERVAL_TIMER_TSCR1_LOAD1_BIT_MASK);
  // Reinitialize the timer.
  intervalTimer_init(timerNumber);
  return 0;
}

// Must call this before using a timer.
u32 intervalTimer_init(u32 timerNumber) {
  // Configure the timer as follows.
  // Cascade mode, up count.
  // Restore both control registers to zero.
  intervalTimer_writeRegister(timerNumber, INTERVAL_TIMER_TCSRO_REG_OFFSET, 0);
  intervalTimer_writeRegister(timerNumber, INTERVAL_TIMER_TCSR1_REG_OFFSET, 0);
  intervalTimer_writeRegister(timerNumber,
							  INTERVAL_TIMER_TCSRO_REG_OFFSET,
							  INTERVAL_TIMER_TSCR0_CASC_BIT_MASK | INTERVAL_TIMER_TSCR0_UDT0_BIT_MASK);
  return 0;
}

// Just calls intervalTimer_init on all timers.
u32 intervalTimer_initAll() {
  int i;
  for (i=0; i<INTERVAL_TIMER_MAX_TIMER_INDEX; i++) {
    intervalTimer_init(i);
  }
  return 0;
}

// Just calls intervalTimer_reset on all timers.
u32 intervalTimer_resetAll() {
  int i;
  for (i=0; i<INTERVAL_TIMER_MAX_TIMER_INDEX; i++) {
    intervalTimer_reset(i);
  }
  return 0;
}

// Just calls intervalTimer_test on all timers.
u32 intervalTimer_testAll() {
  u32 error = 0;
  int i;
  for (i=0; i<INTERVAL_TIMER_MAX_TIMER_INDEX; i++) {
    if (intervalTimer_test(i))
      error = 1;
  }
  return error;
}


//
// This will configure the counter, start it, see if it runs, stop it, see if it stays stopped.
// Pretty good way to test for the existence of the counter hardware.
u32 intervalTimer_test(u32 timerNumber) {
  u32 errorValue = 0;
  // See if the hardware appears to be present by writing a value to the load register and reading it back.
  u32 loadValue = 0x55AA;
  intervalTimer_writeRegister(timerNumber, INTERVAL_TIMER_TLR0_REG_OFFSET, loadValue);
  if (loadValue != intervalTimer_readRegister(timerNumber, INTERVAL_TIMER_TLR0_REG_OFFSET)) {
    printf("intervalTimer_test: hardware for timer: %lu does not appear to be present.\n\r", timerNumber);
  }
  intervalTimer_init(timerNumber);  // Init the timer.
  intervalTimer_reset(timerNumber); // Reset the timer to 0.
  u32 counter0Value = intervalTimer_readRegister(timerNumber, INTERVAL_TIMER_TCR0_REG_OFFSET);
  // See if the timer is reset. Timer should be zero at this point.
  if (counter0Value) {
    printf("intervalTimer_test: counter 0 should be zero after initialization. Value is: %lu.\n", counter0Value);
    errorValue = 1;
  } else {
	// OK, see if the timer will run.
    intervalTimer_start(timerNumber);
    volatile int i;  // Wait for a bit.
    for (i=0; i<1000; i++);
    counter0Value = intervalTimer_readRegister(timerNumber, INTERVAL_TIMER_TCR0_REG_OFFSET);
    if (!counter0Value) {
	  printf("intervalTimer_test: counter 0 does not seem to be running. Value is: %lu.\n\r", counter0Value);
	  errorValue = 1;
    } else {
      // OK. See if the timer will stop.
      intervalTimer_stop(timerNumber);
      counter0Value = intervalTimer_readRegister(timerNumber, INTERVAL_TIMER_TCR0_REG_OFFSET);
      int i;  // Wait for a bit.
      for (i=0; i<1000; i++);
      u32 secondCounter0Value = intervalTimer_readRegister(timerNumber, INTERVAL_TIMER_TCR0_REG_OFFSET);
      if (counter0Value != secondCounter0Value) {
  	    printf("intervalTimer_test: counter 0 does not seem to be stopped.\n\r");
	    errorValue = 1;
      }
    }
  }
  return errorValue;
}

// Returns the frequency for each timer (from xparameters.h).
u32 intervalTimer_getTimerFrequency(u32 timerNumber) {
  switch(timerNumber) {
  case 0:
	  return XPAR_AXI_TIMER_0_CLOCK_FREQ_HZ;
  case 1:
	  return XPAR_AXI_TIMER_1_CLOCK_FREQ_HZ;
  case 2:
	  return XPAR_AXI_TIMER_2_CLOCK_FREQ_HZ;
  default:
	printf("intervalTimer_getTimerFrequency: timerNumber (%ld) is not between 0 and %d.\n\r",
			timerNumber, INTERVAL_TIMER_MAX_TIMER_INDEX);
	return 0;
  }
}

// This routine assumes that the counter may be running. This could be simplified if will only
// invoke this function when the counter is not running.
u64 intervalTimer_get64BitCounterValue(u32 timerNumber) {
  u32 upperCounterValue = intervalTimer_readRegister(timerNumber, INTERVAL_TIMER_TCR1_REG_OFFSET);
  u32 lowerCounterValue = intervalTimer_readRegister(timerNumber, INTERVAL_TIMER_TCR0_REG_OFFSET);
  u32 secondUpperCounterValue = intervalTimer_readRegister(timerNumber,INTERVAL_TIMER_TCR1_REG_OFFSET);
  if (upperCounterValue != secondUpperCounterValue) {
    lowerCounterValue = intervalTimer_readRegister(timerNumber, INTERVAL_TIMER_TCR0_REG_OFFSET);
    upperCounterValue = secondUpperCounterValue;
  }
  u64 counterValue = upperCounterValue;
  counterValue <<= 32;
  counterValue |= lowerCounterValue;
  return counterValue;
}

// Just returns the lower 32 bit value of the counter. Useful for generating random number generator seeds.
u32 intervalTimer_getLower32BitCounterValue(u32 timerNumber) {
	return intervalTimer_readRegister(timerNumber, INTERVAL_TIMER_TCR0_REG_OFFSET);
}

u32 intervalTimer_getTotalDurationInSeconds(u32 timerNumber, double *seconds) {
  double timerFrequency = intervalTimer_getTimerFrequency(timerNumber);
  double counterValue = intervalTimer_get64BitCounterValue(timerNumber);
  *seconds = counterValue / timerFrequency;
  return 0;
}


