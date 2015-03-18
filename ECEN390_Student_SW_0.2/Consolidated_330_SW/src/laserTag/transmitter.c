/*
 * transmitter.c
 *
 *  Created on: Dec 22, 2014
 *      Author: hutch
 */

#include "transmitter.h"
#include <stdint.h>
#include "supportFiles/buttons.h"
#include "supportFiles/switches.h"
#include "supportFiles/mio.h"
#include "supportFiles/utils.h"
#include <stdio.h>

#define TRANSMITTER_OUTPUT_PIN 13
#define TRANSMITTER_HIGH_VALUE 1
#define TRANSMITTER_LOW_VALUE 0
#define PULSE_LENGTH 20000
#define PLAYER_FREQUENCIES 10

// States for the controller state machine.
enum transmitterStates {
	init_st,                 // Start here, stay in this state for just one tick.
	high_st,
	low_st
} transmitterState = init_st;

static bool enableFlag = false;
static uint16_t count = 0;
static uint8_t freqIndex = 0;

const uint8_t freq[PLAYER_FREQUENCIES] = {45,36,29,25,22,19,17,15,14,13};

void transmitter_init() {
	mio_init(false);  // false disables any debug printing if there is a system failure during init.
	mio_setPinAsOutput(TRANSMITTER_OUTPUT_PIN);  // Configure the signal direction of the pin to be an output.
}

void transmitter_set_jf1_to_one() {
	mio_writePin(TRANSMITTER_OUTPUT_PIN, TRANSMITTER_HIGH_VALUE); // Write a '1' to JF-1.
}

void transmitter_set_jf1_to_zero() {
	mio_writePin(TRANSMITTER_OUTPUT_PIN, TRANSMITTER_LOW_VALUE); // Write a '1' to JF-1.
}

// Starts the transmitter. Does nothing if the transmitter is already running.
void transmitter_run() {
	enableFlag = true;
}

// Returns true if the transmitter is running.
bool transmitter_running() {
	return enableFlag;
}

// Sets the frequency number. If this function is called while the
// transmitter is running, the frequency will not be updated until the
// transmitter stops and transmitter_run() is called again.
void transmitter_setFrequencyNumber(uint16_t frequencyNumber) {
	if(!transmitter_running())
		freqIndex = frequencyNumber;
}

// Standard tick function.
void transmitter_tick() {
	// Perform state action first.
	switch(transmitterState) {
	case init_st:
		break;
	case high_st:
		count++;
		break;
	case low_st:
		count++;
		break;
	default:
		printf("transmitter_tick state action: hit default\n\r");
		break;
	}

	// Perform state update next.
	switch(transmitterState) {
	case init_st:
		if(enableFlag) {
			transmitterState = high_st;
			transmitter_set_jf1_to_one();
			count = 0;
		}
		break;
	case high_st:
		if(count == PULSE_LENGTH) {
			enableFlag = false;
			transmitter_set_jf1_to_zero();
			transmitterState = init_st;
		} else if(!(count % freq[freqIndex])) {
			transmitter_set_jf1_to_zero();
			transmitterState = low_st;
		}
		break;
	case low_st:
		if(count == PULSE_LENGTH) {
			enableFlag = false;
			transmitterState = init_st;
		} else if(!(count % freq[freqIndex])) {
			transmitter_set_jf1_to_one();
			transmitterState = high_st;
		}
		break;
	default:
		printf("transmitter_tick state update: hit default\n\r");
		break;
	}
}

// Tests the transmitter.
void transmitter_runTest() {
	printf("Transmitter Run Test\n\r");
	while(!buttons_read()) {
		transmitter_setFrequencyNumber(switches_read());
		transmitter_run();
		utils_msDelay(300);
	}
}

















