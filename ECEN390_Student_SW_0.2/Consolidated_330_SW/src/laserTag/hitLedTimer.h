/*
 * hitLedTimer.h
 *
 *  Created on: Mar 8, 2015
 *      Author: DJ
 */

#ifndef HITLEDTIMER_H_
#define HITLEDTIMER_H_

// Need to init things.
void hitLedTimer_init();

// Calling this starts the timer.
void hitLedTimer_start();

// Returns true if the timer is currently running.
bool hitLedTimer_running();

// Standard tick function.
void hitLedTimer_tick();

void hitLedTimer_runTest();


#endif /* HITLEDTIMER_H_ */
