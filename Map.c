// Implementation of the Map ADT

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Map.h"

struct map { 
    // TODO
    int cities;
    int numRoads;
    struct road **roads; // adjacency list for roads
    int *numCityRoads; // num roads from a city
    char **cityNames; // id of cityName is the index that contains the name in array
};

static void addRoad(Map m, int from, int to, int length);
static int sortRoadsByTo(const void *a, const void *b);

///////////////////////////////////////////////////////////////////////////////////////////////////
Map MapNew(int numCities) {
    // TODO
    Map m = malloc(sizeof(struct map));
    if (m == NULL) {
        fprintf(stderr, "error: out of memory\n");
        exit(EXIT_FAILURE);
    }

    m->cityNames = calloc(numCities, sizeof(char *));
    if (m->cityNames == NULL) {
        fprintf(stderr, "error: out of memory\n");
        exit(EXIT_FAILURE);
    }

    m->roads = calloc(numCities, sizeof(struct road *));
    if (m->roads == NULL) {
        fprintf(stderr, "error: out of memory\n");
        exit(EXIT_FAILURE);
    }

    m->numCityRoads = calloc(numCities, sizeof(struct road *));
    if (m->numCityRoads == NULL) {
        fprintf(stderr, "error: out of memory\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < numCities; i++) {
        m->cityNames[i] = NULL;
        m->roads[i] = calloc(numCities, sizeof(struct road));
        if(m->roads[i] == NULL) {
            fprintf(stderr, "error: out of memory\n");
            exit(EXIT_FAILURE);
        }

        for (int j = 0; j < numCities; j++) {
            m->roads[i][j].from = i;
            m->roads[i][j].to = j;
            m->roads[i][j].length = 0;
        }
        
    }

    m->cities = numCities;
    m->numRoads = 0;
    
    return m;
}

void MapFree(Map m) {
    // TODO
    for (int i = 0; i < m->cities; i++) {
        free(m->cityNames[i]);
    }
    free(m->cityNames);

    for (int i = 0; i < m->cities; i++) {
        free(m->roads[i]);
    }
    free(m->roads);

    free(m->numCityRoads);
    free(m);
}

int MapNumCities(Map m) {
    // TODO
    if (m == NULL) {
        return 0;
    }

    return m->cities;
}

int MapNumRoads(Map m) {
    // TODO
    if (m == NULL) {
        return 0;
    }

    return m->numRoads;
}

void MapSetName(Map m, int city, char *name) {
    // TODO
    if (m == NULL) {
        return;
    }

    if (m->cityNames[city] != NULL) {
        free(m->cityNames[city]);
    }

    m->cityNames[city] = malloc(strlen(name) + 1);
    if (m->cityNames[city] == NULL) {
        fprintf(stderr, "error: out of memory\n");
        exit(EXIT_FAILURE);
    }

    strcpy(m->cityNames[city], name);
}

char *MapGetName(Map m, int city) {
    // TODO
    if (m == NULL) {
        return NULL;
    }

    if (m->cityNames[city] == NULL) {
        return "unnamed";
    }
    
    return m->cityNames[city];
}

void MapInsertRoad(Map m, int city1, int city2, int length) {
    // TODO
    if (m == NULL) {
        return;
    }

    addRoad(m, city1, city2, length);
    addRoad(m, city2, city1, length);

    m->numRoads++;
}

int MapContainsRoad(Map m, int city1, int city2) {
    // TODO
    if (m == NULL) {
        return 0;
    }

    if (m->roads[city1][city2].length > 0) {
        return m->roads[city1][city2].length;
    }

    return 0;
}

int MapGetRoadsFrom(Map m, int city, struct road roads[]) {
    // TODO
    if (m == NULL) {
        return 0;
    }

    int roadCount = 0;

    for (int i = 0; i < m->cities; i++) {
        if (m->roads[city][i].length > 0) {
            roads[roadCount].from = city;
            roads[roadCount].to = i;
            roads[roadCount].length = m->roads[city][i].length;
            roadCount++;
        }
    }
    
    qsort(roads, roadCount, sizeof(struct road), sortRoadsByTo);

    return m->numCityRoads[city];
}

/**
 * !!! DO NOT EDIT THIS FUNCTION !!!
 * This function will work once the other functions are working
 */
void MapShow(Map m) {
    printf("Number of cities: %d\n", MapNumCities(m));
    printf("Number of roads: %d\n", MapNumRoads(m));
    
    struct road *roads = malloc(MapNumRoads(m) * sizeof(struct road));
    if (roads == NULL) {
        fprintf(stderr, "error: out of memory\n");
        exit(EXIT_FAILURE);    
    }
    
    for (int i = 0; i < MapNumCities(m); i++) {
        printf("[%d] %s has roads to:", i, MapGetName(m, i));
        int numRoads = MapGetRoadsFrom(m, i, roads);
        for (int j = 0; j < numRoads; j++) {
            if (j > 0) {
                printf(",");
            }
            printf(" [%d] %s (%d)", roads[j].to, MapGetName(m, roads[j].to),
                   roads[j].length);
        }
        printf("\n");
    }
    
    free(roads);
}

/////////////////////////////////////////////// HELPER FUNCTIONS ///////////////////////////////////////

// a helper function that adds road length to the adjacency list and adds to city road count
static void addRoad(Map m, int from, int to, int length) {
    m->roads[from][to].length = length;

    m->numCityRoads[from]++;
}

static int sortRoadsByTo(const void *a, const void *b) {
    const struct road *roadA = (const struct road *)a;
    const struct road *roadB = (const struct road *)b;
    return roadA->to - roadB->to;
}