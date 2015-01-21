/*
 * switches.h
 *
 *  Created on: Sep 3, 2014
 *      Author: hutch
 */

#ifndef SWITCHES_H_
#define SWITCHES_H_

#include <stdint.h>

#define SWITCHES_INIT_STATUS_OK 1
#define SWITCHES_INIT_STATUS_FAIL 0

// Provides functions to read the push SWITCHESs (BTN3, BTN2, BTN1, BTN0)

// Initializes the SWITCHES driver software and hardware.
int switches_init();

// Returns the current value of all 4 SWITCHESs as the lower 4 bits of the returned value.
// bit3 = SW3, bit2 = SW2, bit1 = SW1, bit0 = SW0.
int32_t switches_read();

// Runs a test of the SWITCHESs. As you push the SWITCHESs, messages will be written to the LCD
// panel. The test will run for about 10 seconds.
void switches_runTest();



#endif /* SWITCHES_H_ */
