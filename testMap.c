// Main program for testing the Map ADT

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Map.h"

void testSetName(void);
void testInsertRoad(void);
void testGetRoadsFrom(void);

int main(int argc, char *argv[]) {
	testSetName();
	testInsertRoad();
	testGetRoadsFrom();
}

void testSetName(void) {
	Map m = MapNew(5);

	char name1[] = "sydney";
	assert(strcmp(MapGetName(m, 0), "unnamed") == 0);
	MapSetName(m, 0, name1);

	// Overwriting the name1 string with something random
	strcpy(name1, "???");
	assert(strcmp(MapGetName(m, 0), "sydney") == 0);

	char name2[] = "canberra";
	assert(strcmp(MapGetName(m, 4), "unnamed") == 0);
	MapSetName(m, 4, name2);

	// Overwriting the name2 string with something random
	strcpy(name2, "???");
	assert(strcmp(MapGetName(m, 4), "canberra") == 0);

	// Edge case: re-set name for an already named city
	char newName[] = "melbourne";
    MapSetName(m, 0, newName);
    assert(strcmp(MapGetName(m, 0), "melbourne") == 0);

	MapFree(m);
}

void testInsertRoad(void) {
	Map m = MapNew(5);

	assert(MapNumCities(m) == 5);

	MapInsertRoad(m, 0, 1, 10);
	MapInsertRoad(m, 0, 2, 14);
	MapInsertRoad(m, 1, 3, 13);

	assert(MapNumRoads(m) == 3);

	assert(MapContainsRoad(m, 0, 1) == 10);
	assert(MapContainsRoad(m, 0, 2) == 14);
	assert(MapContainsRoad(m, 0, 3) == 0);

	// Check roads are bidirectional
	assert(MapContainsRoad(m, 1, 0) == 10);	
	assert(MapContainsRoad(m, 2, 0) == 14);

	// Edge case: Inserting a duplicate road with updated length
    MapInsertRoad(m, 0, 1, 20);
    assert(MapContainsRoad(m, 0, 1) == 20);

    // Edge case: Insert a road with min length 
    MapInsertRoad(m, 2, 3, 1);
    assert(MapContainsRoad(m, 2, 3) == 1);

    // Edge case: Multiple Roads for the same city
	MapInsertRoad(m, 0, 2, 15);
    assert(MapContainsRoad(m, 0, 2) == 15);

	assert(MapNumRoads(m) == 6);

	MapFree(m);
}

void testGetRoadsFrom(void) {
	Map m = MapNew(5);

	MapInsertRoad(m, 0, 1, 10);
	MapInsertRoad(m, 0, 2, 14);
	MapInsertRoad(m, 1, 3, 13);

	struct road roads[5];
	int numRoads = MapGetRoadsFrom(m, 0, roads);
	assert(numRoads == 2);

	assert(roads[0].from == 0 && roads[0].to == 1 && roads[0].length == 10);
	assert(roads[1].from == 0 && roads[1].to == 2 && roads[1].length == 14);

	// Edge case: check sorted order by 'to'
	assert(roads[0].to < roads[1].to);

    // Edge case: No roads from a city
    numRoads = MapGetRoadsFrom(m, 4, roads);
    assert(numRoads == 0);

	MapFree(m);
}

