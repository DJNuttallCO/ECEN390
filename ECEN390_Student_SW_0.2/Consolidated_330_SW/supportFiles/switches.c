/*
 * switches.c
 *
 *  Created on: Sep 3, 2014
 *      Author: hutch
 */

#include <stdbool.h>
#include <stdio.h>
#include "switches.h"
#include "supportFiles/leds.h"
#include "supportFiles/display.h"
#include "xil_io.h"

// Just need these two register offsets - not using interrupts.
#define SWITCHES_GPIO_DATA_REGISTER_OFFSET 0
#define SWITCHES_GPIO_TRI_REGISTER_OFFSET 4

#define SWITCHES_GPIO_DEVICE_BASE_ADDRESS XPAR_GPIO_SLIDE_SWITCHES_BASEADDR  // Base address of slide-switch GPIO
#define SWITCHES_INPUT_DATA_DIRECTION_VALUE 0xF  // This value configures GPIO for input.
#define SWITCHES_COUNT 4                // There are 4 push-switches.
#define MAX_SWITCHES_LABEL_LENGTH 5     // 4 characters + NULL, e.g., "BTN0"
#define SWITCHES_LABEL_NUMBER_LENGTH 2  // single number + NULL.
#define SWITCHES_BASE_LABEL "BTN"       // All switches share this base label.
#define SWITCHES_LABEL_X_OFFSET 20      // Offset the label so it falls inside the drawn rect.
#define SWITCHES_LABEL_Y_OFFSET 50      // Offset the label so it falls inside the drawn rect.
#define SWITCHES_TERMINATE_TEST_VALUE 0xF    // Terminate the test if you press all switches at once.
#define ALL_LEDS_OFF 0                  // Writing this to the LEDs turns them all off.

bool switches_initFlag = false;  // If true, init has already been called. Set to true if init has been called.

// Helper function to write registers.
void switches_writeGpioRegister(int32_t offset, int32_t value) {
  // Just use the low-level Xilinx call.
  Xil_Out32(SWITCHES_GPIO_DEVICE_BASE_ADDRESS + offset, value);
}

// Helper function to read registers.
int32_t switches_readGpioRegister(int32_t offset) {
  // Just use the low-level Xilinx call.
  return Xil_In32(SWITCHES_GPIO_DEVICE_BASE_ADDRESS + offset);
}

int switches_init() {
	if (switches_initFlag) return SWITCHES_INIT_STATUS_OK;  // Do nothing if init has already been called.
	switches_initFlag = true;  // OK, init has been called.
	// Check to see that the hardware is installed by writing and then reading the data register.
	// Nothing to actually init on the GPIO hardware.
	// This also sets up the data direction so that it can read the switches (input).
	switches_writeGpioRegister(SWITCHES_GPIO_TRI_REGISTER_OFFSET, SWITCHES_INPUT_DATA_DIRECTION_VALUE);
	// Read back the value and see if it matches what you wrote (pretty limited testing is possible here).
	int32_t testValue = switches_readGpioRegister(SWITCHES_GPIO_TRI_REGISTER_OFFSET);
	if (testValue != SWITCHES_INPUT_DATA_DIRECTION_VALUE) {
		switches_initFlag = true;
		return SWITCHES_INIT_STATUS_OK;
	} else {
		return SWITCHES_INIT_STATUS_FAIL;
	}
}

// Returns the current value of all 4 switches as the lower 4 bits of the returned value.
// bit3 = BTN3, bit2 = BTN2, bit1 = BTN1, bit0 = BTN0.
int32_t switches_read() {
	if (!switches_initFlag) {
		printf("switches_read(): must call switches_init() before calling switches_read()\n\r");
		return 0;
	}
	return switches_readGpioRegister(SWITCHES_GPIO_DATA_REGISTER_OFFSET);  // Just read the data register and return the value.
}

// Copies the switch value to the LEDs. The test terminates when the terminate value is recieved.
void switches_runTest() {
	// Call all of the init functions.
	switches_init();
	leds_init(false);
	// Clear out the LEDs.
	leds_write(ALL_LEDS_OFF);
	while (true) {
		int32_t switchValue = switches_read();  // Read the switches.
		leds_write(switchValue);                // Write the switch value to the LEDs.
		if (switchValue == SWITCHES_TERMINATE_TEST_VALUE) {  // Exit if the switches are set to the termination value.
			leds_write(0); // Clear the leds at the end of the test.
			return;
		}
	}
}



