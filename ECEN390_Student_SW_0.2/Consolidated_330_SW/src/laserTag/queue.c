/*
 * queue.c
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

// Standard queue implementation that leaves one spot empty so easier to check for full/empty.
void queue_init(queue_t* q, queue_size_t size) {
	q->indexIn = 0;		// Write index
	q->indexOut = 0;	// Read index
	q->elementCount = 0;
	q->size = size+1;	// Add one additional location for the empty location.
	q->data = (queue_data_t *) malloc(q->size * sizeof(queue_data_t));
	// Not necessary but helpful for debugging
	for (uint i=0; i<q->size; i++) {
		q->data[i] = -1;
	}
}

// Just free the malloc'd storage.
void gueue_garbageCollect(queue_t* q) {
	free(q->data);
}

// Prints in order of array index
void queue_debugPrint(queue_t* q) {
	for (uint i=0; i<q->size; i++) {
		printf("debugData[%d]:%le\n\r", i, q->data[i]);
	}
}

// Returns the size of the queue..
queue_size_t queue_size(queue_t* q) {
	return q->size;
}

// Returns true if the queue is full.
bool queueFull(queue_t* q) {
	if(q->indexIn == q->size - 1) { // -2 because it is the second to last index
		return !q->indexOut; // In this case, only full if Out is at index 0
	} else
		return (q->indexIn + 1) == q->indexOut; // Is it ahead by 1?
}

// Returns true if the queue is empty.
bool queue_empty(queue_t* q) {
	return q->indexIn==q->indexOut;
}

// Helper function to advance the ptrs
queue_index_t queue_advancePtr(queue_index_t p, queue_size_t size) {
	return p == size - 1?0:p+1;	// Also could do a mod...
}

// Pushes a new element into the queue. Reports an error if the queue is full.
void queue_push(queue_t* q, queue_data_t value) {
	if(queueFull(q)) {
		printf("Error: queue_push - full queue, item not pushed\n\r");
		return;
	} else {
		q->data[q->indexIn] = value;
		q->indexIn = queue_advancePtr(q->indexIn, q->size);
		q->elementCount++;
	}
}

// Removes the oldest element in the queue.
queue_data_t queue_pop(queue_t* q) {
	queue_data_t temp = q->data[q->indexOut];
	if(queue_empty(q)) {
		printf("Error: queue_pop - empty queue, pop data invalid\n\r");
	} else {
		q->indexOut = queue_advancePtr(q->indexOut, q->size);
		q->elementCount--;
		//q->data[q->indexOut] = -1; // For debugging purposes only
	}
	return temp;
}

// Pushes a new element into the queue, making room by removing the oldest element.
void queue_overwritePush(queue_t* q, queue_data_t value) {
	queue_pop(q);
	queue_push(q, value);
}

// Provides random-access read capability to the queue.
// Low-valued indexes access older queue elements while higher-value indexes access newer elements
// (according to the order that they were added).
queue_data_t queue_readElementAt(queue_t* q, queue_index_t index) {
	return q->data[(index + q->indexOut) % q->size];
}

// Returns a count of the elements currently contained in the queue.
queue_size_t queue_elementCount(queue_t* q) {
	return q->elementCount;
}

// Prints the current contents of the queue. Handy for debugging.
void queue_print(queue_t* q) {
	for (uint i=0; i<queue_elementCount(q); i++) {
		printf("data[%d]:%le\n\r", i, queue_readElementAt(q, i));
	}
	printf("\n\r");
}

// Performs a comprehensive test of all queue functions.
int queue_runTest() {
	queue_t q;
	queue_t* qp = &q;
	queue_init(qp,5);
	queue_print(qp);
	queue_push(qp,4);
	queue_print(qp);
	queue_push(qp,6);
	queue_print(qp);
	queue_pop(qp);
	queue_print(qp);
	queue_push(qp,1);
	queue_print(qp);
	queue_push(qp,2);
	queue_print(qp);
	queue_push(qp,8);
	queue_print(qp);
	queue_push(qp,9);
	queue_print(qp);
	queue_push(qp,0);
	queue_print(qp);
	queue_overwritePush(qp,13);
	queue_print(qp);
	queue_overwritePush(qp,43);
	queue_print(qp);
	queue_overwritePush(qp,0);
	queue_print(qp);
	queue_pop(qp);
	queue_print(qp);
	queue_pop(qp);
	queue_print(qp);
	queue_pop(qp);
	queue_print(qp);
	queue_pop(qp);
	queue_print(qp);
	queue_pop(qp);
	queue_print(qp);
	queue_pop(qp);
	queue_print(qp);
	queue_pop(qp);
	queue_print(qp);
	queue_push(qp,8);
	queue_print(qp);
	queue_push(qp,8);
	queue_print(qp);
	printf("popped: %le\n\r", queue_pop(qp));
	return true;
}
