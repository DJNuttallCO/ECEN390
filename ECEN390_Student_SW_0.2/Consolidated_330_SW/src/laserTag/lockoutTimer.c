/*
 * lockoutTimer.c
 *
 *  Created on: Mar 8, 2015
 *      Author: DJ
 */
#include <stdio.h>
#include "supportFiles/buttons.h"
#include "supportFiles/intervalTimer.h"

#define LOCKOUT_TIME 50000

// States for the controller state machine.
enum lockoutStates {
	init_st,
	run_st,
} lockoutState = init_st;

volatile static bool enableFlag = false;
static uint32_t count = 0;

// Standard init function.
void lockoutTimer_init() {

}

// Calling this starts the timer.
void lockoutTimer_start() {
	enableFlag = true;
}

// Returns true if the timer is running.
bool lockoutTimer_running() {
	return enableFlag;
}

// Standard tick function.
void lockoutTimer_tick() {
	// Perform state action first.
	switch(lockoutState) {
	case init_st:
		break;
	case run_st:
		count++;
		break;
	default:
		printf("transmitter_tick state action: hit default\n\r");
		break;
	}

	// Perform state update next.
	switch(lockoutState) {
	case init_st:
		if(enableFlag) {
			lockoutState = run_st;
			count = 0;
		}
		break;
	case run_st:
		if(count == LOCKOUT_TIME) {
			enableFlag = false;
			lockoutState = init_st;
		}
		break;
	default:
		printf("transmitter_tick state update: hit default\n\r");
		break;
	}
}

void lockoutTimer_runTest() {
	printf("Lockout Timer Run Test\n\r");
	double seconds;
	while(!buttons_read()) {
		intervalTimer_reset(2);
		intervalTimer_start(2);
		lockoutTimer_start();
		while(lockoutTimer_running());
		intervalTimer_stop(2);
		intervalTimer_getTotalDurationInSeconds(2,&seconds);
		printf("Measured time: %e\n\r",seconds);
	}
}

