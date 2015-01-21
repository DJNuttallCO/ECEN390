/*
 * intervalTimer.h
 *
 *  Created on: Apr 2, 2014
 *      Author: hutch
 */

// Provides an API for accessing the three hardware timers that are installed
// in the ZYNQ fabric.

#ifndef INTERVALTIMER_H_
#define INTERVALTIMER_H_

#include "xil_types.h"
// Register Offsets
#define INTERVAL_TIMER_TCSRO_REG_OFFSET 0x00
#define INTERVAL_TIMER_TLR0_REG_OFFSET  0x04
#define INTERVAL_TIMER_TCR0_REG_OFFSET  0x08
#define INTERVAL_TIMER_TCSR1_REG_OFFSET 0x10
#define INTERVAL_TIMER_TLR1_REG_OFFSET  0x14
#define INTERVAL_TIMER_TCR1_REG_OFFSET  0x18

// Register Bit Masks
#define INTERVAL_TIMER_TSCR0_CASC_BIT_MASK  0x0800
#define INTERVAL_TIMER_TSCR0_ENALL_BIT_MASK 0x0400
#define INTERVAL_TIMER_TSCR0_PWMA0_BIT_MASK 0x0200
#define INTERVAL_TIMER_TSCR0_TOINT_BIT_MASK 0x0100
#define INTERVAL_TIMER_TSCR0_ENT0_BIT_MASK  0x0080
#define INTERVAL_TIMER_TSCR0_ENIT0_BIT_MASK 0x0040
#define INTERVAL_TIMER_TSCR0_LOAD0_BIT_MASK 0x0020
#define INTERVAL_TIMER_TSCR0_ARHT0_BIT_MASK 0x0010
#define INTERVAL_TIMER_TSCR0_CAPT0_BIT_MASK 0x0008
#define INTERVAL_TIMER_TSCR0_GENT0_BIT_MASK 0x0004
#define INTERVAL_TIMER_TSCR0_UDT0_BIT_MASK  0x0002
#define INTERVAL_TIMER_TSCR0_MDT0_BIT_MASK  0x0001

#define INTERVAL_TIMER_TSCR1_ENALL_BIT_MASK 0x400
#define INTERVAL_TIMER_TSCR1_PWMA0_BIT_MASK 0x200
#define INTERVAL_TIMER_TSCR1_T1INT_BIT_MASK 0x100
#define INTERVAL_TIMER_TSCR1_ENT1_BIT_MASK  0x080
#define INTERVAL_TIMER_TSCR1_ENIT1_BIT_MASK 0x040
#define INTERVAL_TIMER_TSCR1_LOAD1_BIT_MASK 0x020
#define INTERVAL_TIMER_TSCR1_ARHT1_BIT_MASK 0x010
#define INTERVAL_TIMER_TSCR1_CAPT1_BIT_MASK 0x008
#define INTERVAL_TIMER_TSCR1_GENT1_BIT_MASK 0x004
#define INTERVAL_TIMER_TSCR1_UDT1_BIT_MASK  0x002
#define INTERVAL_TIMER_TSCR1_MDT1_BIT_MASK  0x001

#define INTERVAL_TIMER_64_BIT_COUNTER_START_COMMAND (INTERVAL_TIMER_TSCR0_CASC_BIT_MASK | INTERVAL_TIMER_TSCR0_ENT0_BIT_MASK)
#define INTERVAL_TIMER_64_BIT_COUNTER_STOP_COMMAND (INTERVAL_TIMER_TSCR0_CASC_BIT_MASK)
#define INTERVAL_TIMER_0_TSCR0_REGISTER_ADDRESS (XPAR_AXI_TIMER_0_BASEADDR+ INTERVAL_TIMER_TCSRO_REG_OFFSET)

// Utility defines
#define INTERVAL_TIMER_MAX_TIMER_INDEX 2  // Maximum number of timers, zero-based.

u32 intervalTimer_start(u32 timerNumber);
u32 intervalTimer_stop(u32 timerNumber);
u32 intervalTimer_reset(u32 timerNumber);
u32 intervalTimer_init(u32 timerNumber);
u32 intervalTimer_initAll();
u32 intervalTimer_resetAll();
u32 intervalTimer_testAll();
u32 intervalTimer_test(u32 timerNumber);
u32 intervalTimer_getLower32BitCounterValue(u32 timerNumber);
u32 intervalTimer_getTotalDurationInSeconds(u32 timerNumber, double *seconds);

#define intervalTimer_startTimer0() \
  *(volatile u32 *) INTERVAL_TIMER_0_TSCR0_REGISTER_ADDRESS = INTERVAL_TIMER_64_BIT_COUNTER_START_COMMAND
//  *(volatile u32 *) INTERVAL_TIMER_0_TSCR0_REGISTER_ADDRESS = INTERVAL_TIMER_64_BIT_COUNTER_START_COMMAND;

#define intervalTimer_stopTimer0() \
  *(volatile u32 *) INTERVAL_TIMER_0_TSCR0_REGISTER_ADDRESS = INTERVAL_TIMER_64_BIT_COUNTER_STOP_COMMAND



#endif /* INTERVALTIMER_H_ */
