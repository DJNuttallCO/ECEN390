/*
 * detector.c
 *
 *  Created on: Dec 22, 2014
 *      Author: hutch
 */

#include "supportFiles/interrupts.h"
#include <stdint.h>
#include "isr.h"
#include "stdio.h"
#include "detector.h"
#include "filter.h"
#include "lockoutTimer.h"
#include "hitLedTimer.h"
#include "math.h"

#define ADC_MAX 4095
#define SCALED_WIDTH 2
#define SCALED_OFFSET 1
#define FUDGE_FACTOR 5
#define MEDIAN_INDEX FILTER_IIR_FILTER_COUNT/2 - 1
#define TEST_DATA_COUNT 200
#define TRANSMITTER_TICK_MULTIPLIER 3	// Call the tick function this many times for each ADC interrupt.

const double inputData[TEST_DATA_COUNT] = {1181,1421,1518,1394,1223,1305,1300,1100,1157,1054,1436,1137,1134,1305,1066,1059,1219,1372,1037,1266,1102,1127,977,1387,1496,1029,1261,1337,1326,1346,1314,1170,1224,1099,1544,1144,1071,1276,1402,1204,1270,1238,1066,1325,1076,1136,1262,1215,1275,1287,1088,1006,1422,1308,1201,1443,966,1254,1244,1507,1283,1364,1494,1069,945,1236,1183,1223,1177,986,1258,1176,1337,1376,1308,1045,1098,1350,1017,1176,1120,1123,1116,1161,1313,1138,897,989,1422,1332,1199,1305,1356,1202,1309,1268,1261,1274,1051,1310,1023,1109,1164,1281,1356,1231,1073,1207,1373,1156,1243,1453,1208,1451,1313,1249,1183,1397,1269,1043,1232,1230,1252,1386,1480,1303,1419,1084,1343,1318,1361,1358,1025,1277,1350,1049,1195,1133,1106,1371,953,1129,1300,1395,1520,1220,1335,1301,1113,1400,1242,1395,1111,1287,1240,1528,1422,1208,1009,1315,1261,1456,941,1327,1149,1345,1064,1129,1173,1259,1535,1285,1313,1357,1074,1200,1181,1104,1409,1295,1450,1388,1235,1397,1305,1724,1310,1307,1153,1111,1378,1124,1205,999,970,1349,1307,1147,1381,1180};
const double computedMedian[TEST_DATA_COUNT/10] = {};

static double sortedPower[FILTER_IIR_FILTER_COUNT] = {};
static uint8_t sampleCount = 0; // This may need to be a global so it is not reset to zero every time detector() is called
static bool detector_hitDetectedFlag = false;
static detector_hitCount_t detector_hitArray[FILTER_IIR_FILTER_COUNT] = {0};

void detector_tick() {
}

// Always have to init things.
void detector_init() {
	filter_init();
	lockoutTimer_init();
}

void detector_sort() {
	//	 for i = 1 to length(A) - 1
	//	    x = A[i]
	//	    j = i
	//	    while j > 0 and A[j-1] > x
	//	        A[j] = A[j-1]
	//	        j = j - 1
	// 	    A[j] = x

	// Copy power array
	for(uint8_t i = 0; i < FILTER_IIR_FILTER_COUNT; i++) {
		sortedPower[i] = filter_getCurrentPowerValue(i);
	}
	// Sort
	for(uint8_t i = 1; i < FILTER_IIR_FILTER_COUNT; i++) {
		double x = sortedPower[i];
		uint8_t j = i;
		while(j > 0 && sortedPower[j-1] > x) {
			sortedPower[j] = sortedPower[j-1];
			j = j-1;
		}
		sortedPower[j] = x;
	}
}

// Helper function I made
void detector_computeHit() {
	detector_sort();
	// Multiply the median value with a “fudge-factor” to compute a threshold.
	double threshold = sortedPower[MEDIAN_INDEX] * FUDGE_FACTOR;
	//printf("SortedPower at median_index %d is %f Threshold is %f\n\r", MEDIAN_INDEX,sortedPower[MEDIAN_INDEX], threshold);
	// Find the band-pass filter that contains the maximum power.
	// If the maximum power exceeds the threshold, you have detected a hit.
	if(sortedPower[FILTER_IIR_FILTER_COUNT-1] > threshold)
		detector_hitDetectedFlag = true;
}

// Runs the entire detector: decimating fir-filter, iir-filters, power-computation, hit-detection.
void detector() {
	// Query the adcQueue to determine how many elements it contains.
	// Use the ( isr_adcBufferElementCount() for this). Call this amount elementCount.
	uint32_t elementCount = isr_adcBufferElementCount();
	// Now, repeat the following steps elementCount times.
	for(uint32_t i = 0; i < elementCount; i++) {
		// Briefly disable interrupts by invoking interrupts_disableArmInts().
		interrupts_disableArmInts();
		// Pop a value from adcQueue (use isr_removeDataFromAdcBuffer() for this).
		// Place this value in a variable called rawAdcValue.
		uint32_t rawAdcValue = isr_removeDataFromAdcBuffer();
		// Re-enable interrupts by invoking interrupts_enableArmInts().
		interrupts_enableArmInts();
		// Scale the integer value contained in rawAdcValue to a double that is between -1.0 and 1.0.
		// Store this value into a variable named scaledAdcValue.
		double scaledAdcValue = (rawAdcValue * 1.0 / ADC_MAX) * SCALED_WIDTH - SCALED_OFFSET;
		// Invoke filter_addNewInput(scaledAdcValue). This provides a new input to the decimating FIR filter.
		filter_addNewInput(scaledAdcValue);
		sampleCount++;
		// Perform the decimating FIR filter, IIR filter and power computation for all 10 channels.
		// Remember to only invoke these filters after filter_addNewInput() has been called 10 times (decimation).
		if (sampleCount == FILTER_FIR_DECIMATION_FACTOR) {  // Only invoke the filters after every DECIMATION_FACTOR times.
			sampleCount = 0;                                  // Reset the sample count when you run the filters.
			filter_firFilter();
			for(uint8_t i = 0; i < FILTER_IIR_FILTER_COUNT; i++) {
				filter_iirFilter(i);
				filter_computePower(i,false,false);
			}
			// Pretty sure this should be here and not one bracket down.
			if(!lockoutTimer_running()){
				// If the lockoutTimer is not running, run the previously-described detection algorithm.
				// Sort the power values in ascending order according to their magnitude.
				detector_computeHit();
				// If you detect a hit:
				if(detector_hitDetectedFlag) {
					// Start the lockoutTimer.
					lockoutTimer_start();
					// Start the hitLedTimer.
					hitLedTimer_start();
					// Increment detector_hitArray at the index of the frequency of the IIR-filter output where you detected the hit.
					// Find the maxIndex
					uint8_t maxIndex = 0;
					for(uint8_t i = 0; i < FILTER_IIR_FILTER_COUNT; i++) {
						if(sortedPower[FILTER_IIR_FILTER_COUNT-1] == filter_getCurrentPowerValue(i)) {
							maxIndex = i;
							return;
						}
					}
					detector_hitArray[maxIndex]++;
					// Set detector_hitDetectedFlag to true.
					detector_hitDetectedFlag = true;
				}
			}
		}
	}
}

// Invoke to determine if a hit has occurred.
bool detector_hitDetected() {
	return detector_hitDetectedFlag;
}

// Clear the detected hit once you have accounted for it.
void detector_clearHit() {
	detector_hitDetectedFlag = false;
}

// Get the current hit counts.
void detector_getHitCounts(detector_hitCount_t hitArray[]) {
	for(uint8_t i = 0; i < FILTER_IIR_FILTER_COUNT; i++) {
		hitArray[i] = detector_hitArray[i];
	}
}

// When invoked, this function will test the functionality of your detector software.
// This test relies on the presence of transmitter test data for each channel.
void detector_runTest() {
	//	for(uint8_t i = 0; i < FILTER_IIR_FILTER_COUNT; i++) {
	//		filter_forceValueIntoPowerArray(10*i, i);
	//	}
	// Test data 1 (from wiki)
	static bool success = true;
	printf("Isolated Tests\n\r");
	printf("Warning: fudge factor must be 5 for these tests to pass\n\r");
	printf("Data set 1: The following data should detect a hit.\n\r");
	filter_forceValueIntoPowerArray(150,0);
	filter_forceValueIntoPowerArray(20,1);
	filter_forceValueIntoPowerArray(40,2);
	filter_forceValueIntoPowerArray(10,3);
	filter_forceValueIntoPowerArray(15,4);
	filter_forceValueIntoPowerArray(30,5);
	filter_forceValueIntoPowerArray(35,6);
	filter_forceValueIntoPowerArray(15,7);
	filter_forceValueIntoPowerArray(25,8);
	filter_forceValueIntoPowerArray(80,9);
	detector_computeHit();
//	for(int i = 0; i < FILTER_IIR_FILTER_COUNT; i++) {
//		printf("Filter %d: %f\n\r",i,sortedPower[i]);
//	}
	if(detector_hitDetected())
		printf("Data set 1: Success!\n\r");
	else
		printf("Data set 1: Fail!\n\r");

	// Test data 2
	detector_clearHit();
	printf("Data set 2: The following data should not detect a hit.\n\r");
	filter_forceValueIntoPowerArray(10,2);
	filter_forceValueIntoPowerArray(25,1);
	filter_forceValueIntoPowerArray(30,4);
	filter_forceValueIntoPowerArray(30,7);
	filter_forceValueIntoPowerArray(45,8);
	filter_forceValueIntoPowerArray(50,6);
	filter_forceValueIntoPowerArray(55,5);
	filter_forceValueIntoPowerArray(65,3);
	filter_forceValueIntoPowerArray(70,9);
	filter_forceValueIntoPowerArray(150,0);
//	for(int i = 0; i < FILTER_IIR_FILTER_COUNT; i++) {
//		printf("Filter %d: %f\n\r",i,sortedPower[i]);
//	}
	detector_computeHit();
//	for(int i = 0; i < FILTER_IIR_FILTER_COUNT; i++) {
//		printf("Filter %d: %f\n\r",i,sortedPower[i]);
//	}
	if(!detector_hitDetected())
		printf("Data set 2: Success!\n\r");
	else
		printf("Data set 2: Fail!\n\r");
	printf("\n\rComprehensive test\n\r");
//		uint16_t failedIndex = 0;  // Keep track of the index where things failed.
//		int sampleCount = 0;
//		for (uint16_t i=0; i<TEST_DATA_COUNT; i++) {
//			// No decimation for this test - just invoke the FIR filter each time you add new data.
//			filter_addNewInput(inputData[i]);
//			sampleCount++;                                      // Keep track of how many samples you have acquired.
//			if (sampleCount == FILTER_FIR_DECIMATION_FACTOR) {  // Only invoke the filters after every DECIMATION_FACTOR times.
//				sampleCount = 0;                                  // Reset the sample count when you run the filters.
//				// You may need to perform the equality comparison differently as
//				// filter outputs and test data may not match exactly. If you are within 0.000001, you are probably close enough.
//				filter_firFilter();
//				for(uint8_t j = 0; j < FILTER_IIR_FILTER_COUNT; j++) {
//					filter_iirFilter(j);
//					filter_computePower(j,false,false);
//				}
//				detector_sort();
//				if(sortedPower[MEDIAN_INDEX] != computedMedian[i/10]) {
//					success = false;
//					failedIndex = i/10;
//				}
//			}
//		}
//		if (success) {
//			printf("Success!\n\r");
//		}
//		else {
//			printf("Failure!\n\r");
//			printf("First failure detected at index: %d\n\r", failedIndex);
//		}
}


