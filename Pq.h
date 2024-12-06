// Priority queue of edges
// Edges with smaller weight have higher priority

// !!! DO NOT MODIFY THIS FILE !!!

#ifndef PQ_H
#define PQ_H

#include <stdbool.h>

#include "Map.h"

typedef struct pq *Pq;

/**
 * Creates a new priority queue
 * Time complexity: O(1)
 */
Pq PqNew(void);

/**
 * Frees all memory associated with the given priority queue
 * Time complexity: O(1)
 */
void PqFree(Pq pq);

/**
 * Adds an edge to the priority queue
 * Time complexity: O(log n) amortised
 */
void PqInsert(Pq pq, struct road r);

/**
 * Removes and returns the edge with the smallest weight from the
 * priority queue. If multiple edges have the same smallest weight, one
 * of them will be removed.
 * Time complexity: O(log n)
 */
struct road PqExtract(Pq pq);

/**
 * Returns true if the given priority queue is empty, or false otherwise
 * Time complexity: O(1)
 */
bool PqIsEmpty(Pq pq);

/**
 * Prints the given priority queue to stdout for debugging purposes
 */
void PqShow(Pq pq);

#endif
