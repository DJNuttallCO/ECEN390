/*
 * transmitter.h
 *
 *  Created on: Dec 22, 2014
 *      Author: hutch
 */

#ifndef TRANSMITTER_H_
#define TRANSMITTER_H_

#define TRANSMITTER_OUTPUT_PIN 13	// JF1 (pg. 25 of ZYBO reference manual).
#include <stdint.h>

void transmitter_init();
void transmitter_stop();
void transmitter_start();


void transmitter_setFrequencyNumber(uint16_t frequencyNumber);
void transmitter_tick();
void transmitter_runTest();

#endif /* TRANSMITTER_H_ */
