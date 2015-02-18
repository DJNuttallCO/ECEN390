/*
 * filter.c
 *
 *  Created on: Feb 12, 2015
 *      Author: DJ
 */

#include "filter.h"
#include <stdio.h>
#include <stdbool.h>

#define FIR_COEF_COUNT 4
#define IIR_A_COEFFICIENT_COUNT 45
#define IIR_B_COEFFICIENT_COUNT 45
#define X_QUEUE_SIZE FIR_COEF_COUNT
#define Y_QUEUE_SIZE IIR_B_COEFFICIENT_COUNT
#define Z_QUEUE_SIZE IIR_A_COEFFICIENT_COUNT
#define TEST_DATA_COUNT 5

static queue_t xQueue;
static queue_t yQueue;
static queue_t zQueue[FILTER_IIR_FILTER_COUNT];
const double firBcoeff[FIR_COEF_COUNT] = {};
const double iirAcoeff[FILTER_IIR_FILTER_COUNT][IIR_A_COEFFICIENT_COUNT] = {};
const double iirBcoeff[FILTER_IIR_FILTER_COUNT][IIR_B_COEFFICIENT_COUNT] = {};

// Filtering routines for the laser-tag project.
// Filtering is performed by a two-stage filter, as described below.

// 1. First filter is a decimating FIR filter with a configurable number of taps and decimation factor.
// 2. The output from the decimating FIR filter is passed through a bank of 10 IIR filters. The
// characteristics of the IIR filter are fixed.

// The decimation factor determines how many new samples must be read for each new filter output.
//uint16_t filter_getFirDecimationFactor();

// Must call this prior to using any filter functions.
// Will initialize queues and any other stuff you need to init before using your filters.
// Make sure to fill your queues with zeros after you initialize them.

void initXQueue() {
	queue_init(&xQueue, X_QUEUE_SIZE);
	for (int j=0; j<X_QUEUE_SIZE; j++)
		queue_overwritePush(&xQueue, 0.0);
}

void initYQueue() {
	queue_init(&yQueue, Y_QUEUE_SIZE);
	for (int j=0; j<Y_QUEUE_SIZE; j++)
		queue_overwritePush(&yQueue, 0.0);
}

void initZQueues() {
	for (int i=0; i<FILTER_IIR_FILTER_COUNT; i++) {
		queue_init(&(zQueue[i]), Z_QUEUE_SIZE);
		for (int j=0; j<Z_QUEUE_SIZE; j++)
			queue_overwritePush(&(zQueue[i]), 0.0);
	}
}

void filter_init() {
	// Init queues and fill them with 0s.
	initXQueue();  // Call queue_init() on xQueue and fill it with zeros.
	initYQueue();  // Call queue_init() on yQueue and fill it with zeros.
	initZQueues(); // Call queue_init() on all of the zQueues and fill each z queue with zeros.
}

// Print out the contents of the xQueue for debugging purposes.
void filter_printXQueue() {
	queue_print(&xQueue);
}

// Print out the contents of yQueue for debugging purposes.
void filter_printYQueue() {
	queue_print(&yQueue);
}

// Print out the contents of the the specified zQueue for debugging purposes.
void filter_printZQueue(uint16_t filterNumber) {
	queue_print(&(zQueue[filterNumber]));
}

// Use this to copy an input into the input queue (x_queue).
void filter_addNewInput(double x) {
	queue_overwritePush(&xQueue, x);
}

// Invokes the FIR filter. Returns the output from the FIR filter. Also adds the output to the y_queue for use by the IIR filter.
double filter_firFilter() {
	// += accumulates the result during the for-loop. Must start out with y = 0.
	double y = 0.0;
	// This for-loop performs the identical computation to that shown above. for-loop is correct way to do it.
	for (int i=0; i<FIR_COEF_COUNT; i++) {
		y += queue_readElementAt(&xQueue, i) * firBcoeff[FIR_COEF_COUNT-1-i];  // iteratively adds the (b * input) products.
	}
	queue_overwritePush(&yQueue,y);
	return y;	// Might be wrong
}

// Use this to invoke a single iir filter. Uses the y_queue and z_queues as input. Returns the IIR-filter output.
double filter_iirFilter(uint16_t filterNumber) {
	double aSum = 0.0, bSum = 0.0, z = 0.0;;
	for (int i=0; i<IIR_B_COEFFICIENT_COUNT; i++) {
		bSum += queue_readElementAt(&yQueue, i) * iirBcoeff[filterNumber][IIR_B_COEFFICIENT_COUNT-1-i];  // iteratively adds the (b * input) products.
	}
	for (int i=0; i<FIR_COEF_COUNT; i++) {
		aSum += queue_readElementAt(&(zQueue[filterNumber]), i) * iirAcoeff[filterNumber][IIR_A_COEFFICIENT_COUNT-1-i];  // iteratively adds the (b * input) products.
	}
	z = bSum - aSum;
	queue_overwritePush(&yQueue,z);
	return 0;
}

// Use this to compute the power for values contained in a queue.
// If force == true, then recompute everything from scratch.
double filter_computePower(uint16_t filterNumber, bool forceComputeFromScratch, bool debugPrint) {
	return 0;
}

double filter_getCurrentPowerValue(uint16_t filterNumber) {
	return 0;
}

// Uses the last computed-power values, scales them to the provided lowerBound and upperBound args, returns the index of element containing the max value.
// The caller provides the normalizedArray that will contain the normalized values. indexOfMaxValue indicates the channel with max. power.
void filter_getNormalizedPowerValues(double normalizedArray[], uint16_t* indexOfMaxValue) {

}

void filter_runTest() {
	const double firInputTestData[TEST_DATA_COUNT] =
	{0.3,
			0.25,
			0.41,
			-0.11,
			-0.5
	};

	const double firOutputTestData[TEST_DATA_COUNT] =
	{0.6,
			0.51,
			0.25,
			-0.78,
			-0.12
	};

	filter_init();             // Standard init function.
	bool success = true;	     // Be optimistic
	uint16_t failedIndex = 0;  // Keep track of the index where things failed.
	for (uint16_t i=0; i<TEST_DATA_COUNT; i++) {
		// No decimation for this test - just invoke the FIR filter each time you add new data.
		filter_addNewInput(firInputTestData[i]);
		// You may need to perform the equality comparison differently as
		// filter outputs and test data may not match exactly. If you are within 0.000001, you are probably close enough.
		if (filter_firFilter() != firOutputTestData[i]) {  // Is the output what you expected?
			success = false;                                 // Nope, note failure and break out of the loop.
			failedIndex = i;                                 // Note the failed index.
			break;
		}
	}
	if (success) {
		printf("Success!\n\r");
	}
	else {
		printf("Failure!\n\r");
		printf("First failure detected at index: %d\n\r", failedIndex);
	}
}
