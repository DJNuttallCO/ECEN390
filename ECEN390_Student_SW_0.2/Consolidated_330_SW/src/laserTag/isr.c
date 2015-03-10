/*
 * timerIsr.c
 *
 *  Created on: Jan 2, 2015
 *      Author: hutch
 */

#include "transmitter.h"
#include "hitLedTimer.h"
#include "lockoutTimer.h"
#include "trigger.h"
#include "supportFiles/interrupts.h"
#include "xsysmon.h"

static uint64_t isr_totalXadcSampleCount = 0;

uint64_t isr_getTotalAdcSampleCount() {return isr_totalXadcSampleCount;}

void isr_init() {
	// Init your data structures here.
}

void isr_function() {
	// Insert timing-critical code here.
	// This code is provided so you can see how things work.
	// You will need to write the code that actually reads the ADC and stores the data in a queue.
	// *********** Use this function to read the ADC: interrupts_getAdcData(); ********
	isr_totalXadcSampleCount++;
	transmitter_tick();
	hitLedTimer_tick();
	lockoutTimer_tick();
	trigger_tick();
}


