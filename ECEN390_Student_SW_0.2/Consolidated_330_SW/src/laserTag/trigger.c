/*
 * trigger.c
 *
 *  Created on: Mar 8, 2015
 *      Author: DJ
 */

#include "trigger.h"
#include "supportFiles/mio.h"
#include <stdio.h>
#include "supportFiles/buttons.h"
#include "transmitter.h"

#define TRIGGER_GUN_TRIGGER_MIO_PIN 10
#define DEBOUNCE_TIME 5000
#define GUN_TRIGGER_PRESSED 1

// States for the controller state machine.
enum triggerStates {
	init_st,
	waitPress_st,
	press_st,
	waitRelease_st,
	release_st
} triggerState = init_st;

static bool enableFlag = false;
static uint32_t count = 0;
static bool ignoreGunInput = false;

// Trigger can be activated by either btn0 or the external gun that is attached to TRIGGER_GUN_TRIGGER_MIO_PIN
// Gun input is ignored if the gun-input is high when the init() function is invoked.
bool triggerPressed() {
	return ((!ignoreGunInput & (mio_readPin(TRIGGER_GUN_TRIGGER_MIO_PIN) == GUN_TRIGGER_PRESSED)) ||
			(buttons_read() & BUTTONS_BTN0_MASK));
}

// Init trigger data-structures.
void trigger_init() {
	mio_setPinAsInput(TRIGGER_GUN_TRIGGER_MIO_PIN);
	// If the trigger is pressed when trigger_init() is called, assume that the gun is not connected and ignore it.
	if (triggerPressed()) {
		ignoreGunInput = true;
	}
}

// Enable the trigger state machine. The state-machine does nothing until it is enabled.
void trigger_enable() {
	if(!transmitter_running())
		enableFlag = true;
}

// Standard tick function.
void trigger_tick() {
	// Perform state action first.
	switch(triggerState) {
	case init_st:
		break;
	case waitPress_st:
		break;
	case press_st:
		if(count < DEBOUNCE_TIME)
			count++;
		break;
	case waitRelease_st:
		break;
	case release_st:
		if(count < DEBOUNCE_TIME)
			count++;
		break;
	default:
		printf("transmitter_tick state action: hit default\n\r");
		break;
	}

	// Perform state update next.
	switch(triggerState) {
	case init_st:
		if(enableFlag) {
			triggerState = waitPress_st;
		}
		break;
	case waitPress_st:
		if(triggerPressed()) {
			triggerState = press_st;
			count = 0;
		}
		break;
	case press_st:
		if(count == DEBOUNCE_TIME) {
			triggerState = waitRelease_st;
			printf("D\n\r");
			transmitter_run();
		}
		break;
	case waitRelease_st:
		if(!triggerPressed()) {
			triggerState = release_st;
			count = 0;
		}
		break;
	case release_st:
		if(count == DEBOUNCE_TIME) {
			enableFlag = false;
			printf("U\n\r");
			triggerState = init_st;
		}
		break;
	default:
		printf("transmitter_tick state update: hit default\n\r");
		break;
	}
}

void trigger_runTest() {
	printf("Trigger Run Test\n\r");
	while(buttons_read()!=0x8) {
		trigger_enable();
	}
}

