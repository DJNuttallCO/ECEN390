/*
 * buttons.c
 *
 *  Created on: Sep 3, 2014
 *      Author: hutch
 */

#include <stdbool.h>
#include <stdio.h>
#include "buttons.h"
#include "supportFiles/display.h"
#include "xil_io.h"

// Just need these two register offsets - not using interrupts.
#define BUTTONS_GPIO_DATA_REGISTER_OFFSET 0
#define BUTTONS_GPIO_TRI_REGISTER_OFFSET 4

#define BUTTONS_GPIO_DEVICE_BASE_ADDRESS XPAR_GPIO_PUSH_BUTTONS_BASEADDR
#define BUTTONS_INPUT_DATA_DIRECTION_VALUE 0xF
#define BUTTON_COUNT 4                // There are 4 push-buttons.
#define MAX_BUTTON_LABEL_LENGTH 5     // 4 characters + NULL, e.g., "BTN0"
#define BUTTON_LABEL_NUMBER_LENGTH 2  // single number + NULL.
#define BUTTON_BASE_LABEL "BTN"       // All buttons share this base label.
#define BUTTON_LABEL_X_OFFSET 20      // Offset the label so it falls inside the drawn rect.
#define BUTTON_LABEL_Y_OFFSET 50      // Offset the label so it falls inside the drawn rect.
#define BUTTONS_TERMINATE_TEST_VALUE 0xF    // Terminate the test if you press all buttons at once.

#define TEXT_SIZE 2  // Size of text for the button labels.

bool buttons_initFlag = false;  // If true, init has already been called. Set to true if init has been called.

// Helper function to write registers.
void buttons_writeGpioRegister(int32_t offset, int32_t value) {
  // Just use the low-level Xilinx call.
  Xil_Out32(BUTTONS_GPIO_DEVICE_BASE_ADDRESS + offset, value);
}

// Helper function to read registers.
int32_t buttons_readGpioRegister(int32_t offset) {
  uint32_t *ptr = (uint32_t *) BUTTONS_GPIO_DEVICE_BASE_ADDRESS + offset;
  return *ptr;
  // Just use the low-level Xilinx call.
  // return Xil_In32(BUTTONS_GPIO_DEVICE_BASE_ADDRESS + offset);
}

int buttons_init() {
	if (buttons_initFlag) return BUTTONS_INIT_STATUS_OK;  // Do nothing if init has already been called.
	buttons_initFlag = true;  // OK, init has been called.
	// Check to see that the hardware is installed by writing and then reading the data register.
	// Nothing to actually init on the GPIO hardware.
	// This also sets up the data direction so that it can read the buttons (input).
	buttons_writeGpioRegister(BUTTONS_GPIO_TRI_REGISTER_OFFSET, BUTTONS_INPUT_DATA_DIRECTION_VALUE);
	// Read back the value and see if it matches what you wrote (pretty limited testing is possible here).
    int32_t testValue = buttons_readGpioRegister(BUTTONS_GPIO_TRI_REGISTER_OFFSET);
    if (testValue != BUTTONS_INPUT_DATA_DIRECTION_VALUE) {
      buttons_initFlag = true;
      return BUTTONS_INIT_STATUS_OK;
    } else {
      return BUTTONS_INIT_STATUS_FAIL;
    }
}

// Returns the current value of all 4 buttons as the lower 4 bits of the returned value.
// bit3 = BTN3, bit2 = BTN2, bit1 = BTN1, bit0 = BTN0.
int32_t buttons_read() {
  if (!buttons_initFlag) {
	  printf("buttons_read(): must call buttons_init() before calling buttons_read()\n\r");
	  return 0;
  }
  return buttons_readGpioRegister(BUTTONS_GPIO_DATA_REGISTER_OFFSET);  // Just read the data register and return the value.
}

#define BTN3_LABEL "3"
#define BTN2_LABEL "2"
#define BTN1_LABEL "1"
#define BTN0_LABEL "0"

// Graphics helper function.
// Draws a square with a label.
// if the undrawFlag is true, it clears the square.
void buttons_drawButtonOnLcd(int8_t buttonNumber, bool drawFlag) {
	int16_t x0, y0, color; // Set the color according to the button number.
	int16_t textColor;
	char buttonLabelNumber[BUTTON_LABEL_NUMBER_LENGTH];   // set the button number in the label here.
	switch(buttonNumber) {
	case 3:  // BTN3
		// origin.
		x0 = 0;
		y0 = 0;
		color = DISPLAY_BLUE;
		textColor = DISPLAY_WHITE;
		strcpy(buttonLabelNumber, BTN3_LABEL);
		break;
	case 2:  // BTN2
		// origin + 1/4 width of screen.
		x0 = display_width()/4;
		y0 = 0;
		color = DISPLAY_RED;
		textColor = DISPLAY_WHITE;
		strcpy(buttonLabelNumber, BTN2_LABEL);
		break;
	case 1:  // BTN1
		// origin + 1/2 width of screen.
		x0 = display_width()/2;
		y0 = 0;
		color = DISPLAY_GREEN;
		textColor = DISPLAY_BLACK;
		strcpy(buttonLabelNumber, BTN1_LABEL);
		break;
	case 0:  // BTN0
		// origin + 3/4 width of screen.
		x0 = 3 * (display_width()/4);
		y0 = 0;
		color = DISPLAY_YELLOW;
		textColor = DISPLAY_BLACK;
		strcpy(buttonLabelNumber, BTN0_LABEL);
		break;
	default:
		printf("buttons_drawButtonOnLcd: unknown button number: %d\n\r", buttonNumber);
		return;  // Return doing nothing.
		break;
	}

	// Create the button label.
	char buttonLabel[MAX_BUTTON_LABEL_LENGTH];  // This is the button label.
	strncpy(buttonLabel, BUTTON_BASE_LABEL, MAX_BUTTON_LABEL_LENGTH);     // This is the base part that all labels share.
	strncat(buttonLabel, buttonLabelNumber, MAX_BUTTON_LABEL_LENGTH);     // Add the button number to the label.
	if (drawFlag) {
	  display_fillRect(x0, y0, display_width()/4, display_height()/2, color);          // Draw the filled rectangle.
	  display_setTextSize(TEXT_SIZE);
	  display_setCursor(x0 + BUTTON_LABEL_X_OFFSET, y0 + BUTTON_LABEL_Y_OFFSET);
	  display_setTextColor(textColor);
	  display_println(buttonLabel);
	} else {
	  display_fillRect(x0, y0, display_width()/4, display_height()/2, DISPLAY_BLACK);  // erase by redrawing black.
	}
}

// Runs a test of the buttons. As you push the buttons, messages will be written to the LCD
// panel. The test will run for about 10 seconds.
void buttons_runTest() {
  buttons_init();  // init the buttons.
  display_init();  // init the display.
  int32_t oldButtonValue=0, newButtonValue=0;  // Keep track of old and new button values to limit screen updates.
  newButtonValue = buttons_read();    // Prime the pump.
  display_fillScreen(DISPLAY_BLACK);  // Initially clear the screen.
  while (true) {
  	newButtonValue = buttons_read();  // get the latest button value.
  	if ((oldButtonValue != newButtonValue) ) {  // Read the buttons and see if anything has changed.
  		for (int i=0; i<BUTTON_COUNT; i++) {  // Check to see which button changed by looking at each bit.
  			if ((newButtonValue ^ oldButtonValue) & (0x1 << i))
  				// i is the index of the button, the shifted and "anded" value is the draw flag, e.g.,
  				// if newButtonValue is a '1' at the i-th position, the button square should be drawn on the LCD.
  				buttons_drawButtonOnLcd(i, (newButtonValue >> i) & 0x1);
  		}
  		if (newButtonValue == BUTTONS_TERMINATE_TEST_VALUE) {  // End the test if the right set of buttons are pressed.
  			display_fillScreen(DISPLAY_BLACK);                 // Blank the screen as you exit the test.
  			return;
  		}
  		oldButtonValue = newButtonValue;  // Update the old button value to the new value for change detection in the next round.
  	}
  }
}



