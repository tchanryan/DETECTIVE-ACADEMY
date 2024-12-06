// Implementation of a priority queue of roads

// !!! DO NOT MODIFY THIS FILE !!!

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "Pq.h"
#include "Map.h"

#define DEFAULT_CAPACITY 64

struct pq {
    struct road *roads;  // Change from struct edge to struct road
    int size;
    int capacity;
};

static void fixUp(Pq pq, int i);
static void fixDown(Pq pq, int i);
static void swap(struct road *items, int i, int j);  

Pq PqNew(void) {
    Pq pq = malloc(sizeof(*pq));
    if (pq == NULL) {
        fprintf(stderr, "Couldn't allocate Pq!\n");
        exit(EXIT_FAILURE);
    }
    
    pq->roads = malloc((DEFAULT_CAPACITY + 1) * sizeof(struct road));  
    if (pq->roads == NULL) {
        fprintf(stderr, "Couldn't allocate Pq!\n");
        exit(EXIT_FAILURE);
    }
    
    pq->size = 0;
    pq->capacity = DEFAULT_CAPACITY;
    return pq;
}

void PqFree(Pq pq) {
    free(pq->roads);  
    free(pq);
}

void PqInsert(Pq pq, struct road r) {  
    // If the Pq is full, expand it (i.e., double its capacity)
    if (pq->size == pq->capacity) {
        pq->capacity *= 2;
        pq->roads = realloc(
            pq->roads,
            (pq->capacity + 1) * sizeof(struct road)  
        );
        if (pq->roads == NULL) {
            fprintf(stderr, "Couldn't expand Pq!\n");
            exit(EXIT_FAILURE);
        }
    }
    
    // Add the new item to the end
    pq->size++;
    pq->roads[pq->size] = r;
    fixUp(pq, pq->size);
}

static void fixUp(Pq pq, int i) {
    while (i > 1 && pq->roads[i].length < pq->roads[i / 2].length) {  
        swap(pq->roads, i, i / 2);
        i = i / 2;
    }
}

struct road PqExtract(Pq pq) {
    assert(pq->size > 0);
    
    struct road r = pq->roads[1];  
    pq->roads[1] = pq->roads[pq->size];
    pq->size--;
    fixDown(pq, 1);
    return r;
}

static void fixDown(Pq pq, int i) {
    struct road *rs = pq->roads; 
    while (true) {
        int j = i;
        int l = 2 * i;
        int r = 2 * i + 1;
        if (l <= pq->size && rs[l].length < rs[j].length) j = l;  // Compare road length
        if (r <= pq->size && rs[r].length < rs[j].length) j = r;  // Compare road length
        if (j == i) break;
        swap(pq->roads, i, j);  // Swap struct road
        i = j;
    }
}

bool PqIsEmpty(Pq pq) {
    return (pq->size == 0);
}

void PqShow(Pq pq) {
    printf("#items = %d\n", pq->size);
    printf("Items:");
    for (int i = 1; i <= pq->size; i++) {
        printf(" (from: %d, to: %d, length: %d)",  // Use fields of struct road
               pq->roads[i].from, pq->roads[i].to, pq->roads[i].length);
    }
    printf("\n");
}

////////////////////////////////////////////////////////////////////////

static void swap(struct road *items, int i, int j) {  
    struct road tmp = items[i];
    items[i] = items[j];
    items[j] = tmp;
}
