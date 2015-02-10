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
	if(queueFull(q))
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

#define SMALL_QUEUE_SIZE 10
#define SMALL_QUEUE_COUNT 10
static queue_t smallQueue[SMALL_QUEUE_COUNT];
static queue_t largeQueue;

// smallQueue[SMALL_QUEUE_COUNT-1] contains newest value, smallQueue[0] contains oldest value.
// Thus smallQueue[0](0) contains oldest value. smallQueue[SMALL_QUEUE_COUNT-1](SMALL_QUEUE_SIZE-1) contains newest value.
// Presumes all queue come initialized full of something (probably zeros).
static double popAndPushFromChainOfSmallQueues(double input) {
	// Grab the oldest value from the oldest small queue before it is "pushed" off.
	double willBePoppedValue = queue_readElementAt(&(smallQueue[0]), 0);
	// Sequentially pop from the next newest queue and push into next oldest queue.
	for (int i=0; i<SMALL_QUEUE_COUNT-1; i++) {
		queue_overwritePush(&(smallQueue[i]), queue_pop(&(smallQueue[i+1])));
	}
	queue_overwritePush(&(smallQueue[SMALL_QUEUE_COUNT-1]), input);
	return willBePoppedValue;
}

static bool compareChainOfSmallQueuesWithLargeQueue(uint16_t iterationCount) {
	bool success = true;
	static uint16_t oldIterationCount;
	static bool firstPass = true;
	// Start comparing the oldest element in the chain of small queues, and the large queue
	// and move towards the newest values.
	for (uint16_t smallQIdx=0; smallQIdx<SMALL_QUEUE_COUNT; smallQIdx++) {
		//queue_print(&(smallQueue[smallQIdx])); // Added by me
		for (uint16_t smallQEltIdx=0; smallQEltIdx<SMALL_QUEUE_SIZE; smallQEltIdx++) {
			double smallQElt = queue_readElementAt(&(smallQueue[smallQIdx]), smallQEltIdx);
			double largeQElt = queue_readElementAt(&largeQueue, (smallQIdx*SMALL_QUEUE_SIZE) + smallQEltIdx);
			if (smallQElt != largeQElt) {
				if (firstPass || (iterationCount != oldIterationCount)) {
					printf("Iteration:%d\n\r", iterationCount);
					oldIterationCount = iterationCount;
					firstPass = false;
				}
				printf("largeQ(%d):%lf", (smallQIdx*SMALL_QUEUE_SIZE) + smallQEltIdx, largeQElt);
				printf(" != ");
				printf("smallQ[%d](%d): %lf\n\r", smallQIdx, smallQEltIdx, smallQElt);
				success = false;
			}
		}
	}
	//getchar();	// Added by me
	return success;
}

#define TEST_ITERATION_COUNT 105
#define FILLER 5
bool queue_runTest() {
	bool success = true;  // Be optimistic.
	// Let's make this a real torture test by testing queues against themselves.
	// Test the queue against an array to make sure there is agreement between the two.
	double testData[SMALL_QUEUE_SIZE + FILLER];
	queue_t q;
	queue_init(&q, SMALL_QUEUE_SIZE);
	// Generate test values and place the values in both the array and the queue.
	for (int i=0; i<SMALL_QUEUE_SIZE + FILLER; i++) {
		double value = (double)rand()/(double)RAND_MAX;
		queue_overwritePush(&q, value);
		testData[i] = value;
	}
	//queue_print(&q); // Added by me
	// Everything is initialized, compare the contents of the queue against the array.
	for (int i=0; i<SMALL_QUEUE_SIZE; i++) {
		double qValue = queue_readElementAt(&q, i);
		if (qValue != testData[i+FILLER]) {
			printf("testData[%d]:%lf != queue_readElementAt(&q, %d):%lf\n\r", i, testData[i+FILLER], i+FILLER, qValue);
			success = false;
		}
	}
	if (!success) {
		printf("Test 1 failed. Array contents not equal to queue contents.\n\r");
	} else {
		printf("Test 1 passed. Array contents match queue contents.\n\r");
	}
	success = true;  // Remain optimistic.
	// Test 2: test a chain of 5 queues against a single large queue that is the same size as the cumulative 5 queues.
	for (int i=0; i<SMALL_QUEUE_COUNT; i++)
		queue_init(&(smallQueue[i]), SMALL_QUEUE_SIZE);
	for (int i=0; i<SMALL_QUEUE_COUNT; i++) {
		for (int j=0; j<SMALL_QUEUE_SIZE; j++)
			queue_overwritePush(&(smallQueue[i]), 0.0);
	}
	queue_init(&largeQueue, SMALL_QUEUE_SIZE * SMALL_QUEUE_COUNT);
	for (int i=0; i<SMALL_QUEUE_SIZE*SMALL_QUEUE_COUNT; i++)
		queue_overwritePush(&largeQueue, 0.0);
	for (int i=0; i<TEST_ITERATION_COUNT; i++) {
		double newInput = (double)rand()/(double)RAND_MAX;
		popAndPushFromChainOfSmallQueues(newInput);
		queue_overwritePush(&largeQueue, newInput);
		if (!compareChainOfSmallQueuesWithLargeQueue(i)) {  // i is passed to print useful debugging messages.
			success = false;
		}
	}

	if (success)
		printf("Test 2 passed. Small chain of queues behaves identical to single large queue.\n\r");
	else
		printf("Test 2 failed. The content of the chained small queues does not match the contents of the large queue.\n\r");
	return success;
}
