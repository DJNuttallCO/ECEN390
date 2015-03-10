/*
 * hitLedTimer.c
 *
 *  Created on: Mar 8, 2015
 *      Author: DJ
 */
#include <stdio.h>
#include "stdint.h"
#include "supportFiles/mio.h"
#include "supportFiles/buttons.h"
#include "supportFiles/utils.h"
#include "supportFiles/leds.h"

#define LED_TIME 50000
#define HIT_LED_PIN 11

// States for the controller state machine.
enum ledStates {
	init_st,
	high_st,
} ledState = init_st;

volatile static bool enableFlag = false;
static uint32_t count = 0;

// Need to init things.
void hitLedTimer_init() {
	mio_init(false);  // false disables any debug printing if there is a system failure during init.
	mio_setPinAsOutput(HIT_LED_PIN);  // Configure the signal direction of the pin to be an output.
}

void hitLedTimer_setLed(int value) {
	mio_writePin(HIT_LED_PIN, value); // Write a '1' to JF-1.
}

// Calling this starts the timer.
void hitLedTimer_start() {
	enableFlag = true;
}

// Returns true if the timer is currently running.
bool hitLedTimer_running() {
	return enableFlag;
}

// Standard tick function.
void hitLedTimer_tick() {
	// Perform state action first.
	switch(ledState) {
	case init_st:
		break;
	case high_st:
		count++;
		break;
	default:
		printf("transmitter_tick state action: hit default\n\r");
		break;
	}

	// Perform state update next.
	switch(ledState) {
	case init_st:
		if(enableFlag) {
			ledState = high_st;
			hitLedTimer_setLed(1);
			leds_write(1);
			count = 0;
		}
		break;
	case high_st:
		if(count == LED_TIME) {
			enableFlag = false;
			hitLedTimer_setLed(0);
			leds_write(0);
			ledState = init_st;
		}
		break;
	default:
		printf("transmitter_tick state update: hit default\n\r");
		break;
	}
}

void hitLedTimer_runTest() {
	printf("Hit LED Run Test\n\r");
	while(!buttons_read()) {
		hitLedTimer_start();
		while(hitLedTimer_running());
		utils_msDelay(500);
	}
}

