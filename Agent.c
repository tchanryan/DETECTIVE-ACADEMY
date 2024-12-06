// Implementation of the Agent ADT

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "Agent.h"
#include "Map.h"
#include "Pq.h"


// This struct stores information about an individual agent and can be
// used to store information that the agent needs to remember.
struct agent {
    char *name;
    int startLocation;
    int location;
    int maxStamina; // max stamina
    int stamina;    // current stamina
    int strategy;
    Map map;
    
    // TODO: Add more fields here as needed

    // STAGE 1
    struct cityVisits *timesVisited;

    // STAGE 2
    int *dfsPath;
    int dfsPathIndex;
    int dfsPathLength;

    // STAGE 3
    int thiefLocation;  
    bool hasThiefLocation;
    int *pathToThief;        
    int pathLength;    
    int pathIndex; 
};

struct cityVisits {
    int cityIndex;
    int visitCount;
};

////////////////////////////////////// CURRENT PROGRESS //////////////////////////////////////////////////
/* Currently stuck on making sure the agent follows the shortest path, calculation of shortest path is correct */
/////////////////////////////////////////////////////////////////////////////////////////////////////////

static struct move chooseRandomMove(Agent agent, Map m);
static int filterRoads(Agent agent, struct road roads[], int numRoads,
                       struct road legalRoads[]);

// TODO: Add more function prototypes here as needed
// Stage 1
static struct move chooseCheapestLeastVisited(Agent agent, Map m);
static int getCityVisitCount(Agent agent, int city);
static void incrementCityVisitCount(Agent agent, int cityIndex);

// Stage 2
static struct move chooseDFS(Agent agent, Map m);
static void dfs(Map m, int currentCity, bool *visited, int *dfsPath, int *dfsPathIndex);
static void initializeDFSPath(Agent agent, Map m);

// Stage 3
static void updatePathToThief(Agent agent, Map m);
static int *dijkstra(Agent agent, Map m, int startCity, int targetCity);

/**
 * Creates a new agent
 */
Agent AgentNew(int start, int stamina, int strategy, Map m, char *name) {
    if (start >= MapNumCities(m)) {
        fprintf(stderr, "error: starting city (%d) is invalid\n", start);
        exit(EXIT_FAILURE);
    }
    
    Agent agent = malloc(sizeof(struct agent));
    if (agent == NULL) {
        fprintf(stderr, "error: out of memory\n");
        exit(EXIT_FAILURE);
    }
    
    agent->startLocation = start;
    agent->location = start;
    agent->maxStamina = stamina;
    agent->stamina = stamina;
    agent->strategy = strategy;
    agent->map = m;
    agent->name = strdup(name);
    if (agent->name == NULL) {
        fprintf(stderr, "error: out of memory\n");
        exit(EXIT_FAILURE);
    }
    
    // STAGE 1 : Cheapest-Least visited
    agent->timesVisited = malloc(MapNumCities(m) * sizeof(struct cityVisits));
    if (agent->timesVisited == NULL) {
        fprintf(stderr, "error: out of memory\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < MapNumCities(m); i++) {
        agent->timesVisited[i].cityIndex = i;
        agent->timesVisited[i].visitCount = 0;
    }
    agent->timesVisited[start].visitCount++;

    // STAGE 2 : DFS Strategy
    if (strategy == DFS) {
        agent->dfsPath = malloc(MapNumCities(m) * sizeof(int));
        if (agent->dfsPath == NULL) {
            fprintf(stderr, "error: out of memory\n");;
            exit(EXIT_FAILURE);
        }
        agent->dfsPathIndex = 0;
        agent->dfsPathLength = 0;
    } else {
        agent->dfsPath = NULL;
    }

    // STAGE 3 : Thief location
    agent->thiefLocation = -1;
    agent->hasThiefLocation = false;
    agent->pathToThief = NULL;          
    agent->pathLength = 0;      
    agent->pathIndex = 0; 

    return agent;
}

/**
 * Frees all memory allocated to the agent
 * NOTE: You should not free the map because the map is owned by the
 *       main program, and the main program will free it
 */
void AgentFree(Agent agent) {
    // TODO: You may need to update this to free any extra memory you use
    if (agent == NULL) {
        return;
    }

    free(agent->timesVisited);
    free(agent->name);

    if (agent->dfsPath != NULL) {
        free(agent->dfsPath);
        agent->dfsPath = NULL;
    }

    if (agent->pathToThief != NULL) {
        free(agent->pathToThief);
        agent->pathToThief = NULL;
    }
    
    free(agent);
}

////////////////////////////////////////////////////////////////////////
// Gets information about the agent
// NOTE: It is expected that these functions do not need to be modified

/**
 * Gets the name of the agent
 */
char *AgentName(Agent agent) {
    return agent->name;
}

/**
 * Gets the current location of the agent
 */
int AgentLocation(Agent agent) {
    return agent->location;
}

/**
 * Gets the amount of stamina the agent currently has
 */
int AgentStamina(Agent agent) {
    return agent->stamina;
}

////////////////////////////////////////////////////////////////////////
// Making moves

/**
 * Calculates the agent's next move
 * NOTE: Does NOT actually carry out the move
 */
struct move AgentGetNextMove(Agent agent, Map m) {
    // TODO: Update this function to handle more strategies
    // WARNING: Do not change the logic of the random strategy

    // If agent has the location of the thief, prioritize shortest path > strategy
    if (agent->hasThiefLocation) {
        // make shortest path if not initialized
        if (agent->pathToThief == NULL) {                   /////// Currently not entering this correctly ////////////
            printf("Path to thief is NULL, recalculating...\n");
            updatePathToThief(agent, m);
        }
        // The agent will follow the path to the thief
        if (agent->pathIndex < agent->pathLength) {
            int nextCity = agent->pathToThief[agent->pathIndex];
            int staminaCost = MapContainsRoad(m, agent->location, nextCity);

            if (staminaCost > 0 && staminaCost <= agent->stamina) {
                agent->pathIndex++;
                return (struct move){nextCity, staminaCost};
            } else {
                return (struct move){agent->location, 0};
            }

            if (agent->location == nextCity) {
                agent->pathIndex++;
            }
        }
    }

    // Agent follows strategies
    if (agent->strategy == STATIONARY) {
        return (struct move){agent->location, 0};
    } else if (agent->strategy == RANDOM) {
        return chooseRandomMove(agent, m);
    } else if (agent->strategy == CHEAPEST_LEAST_VISITED) {
        return chooseCheapestLeastVisited(agent, m);
    } else if (agent->strategy == DFS) {
        if (agent->dfsPathLength == 0) {
            initializeDFSPath(agent, m);
        }
        return chooseDFS(agent, m);
    } else {
        printf("error: strategy not implemented yet\n");
        exit(EXIT_FAILURE);
    }
}

static struct move chooseRandomMove(Agent agent, Map m) {
    struct road *roads = malloc(MapNumCities(m) * sizeof(struct road));
    struct road *legalRoads = malloc(MapNumCities(m) * sizeof(struct road));

    // Get all roads to adjacent cities
    int numRoads = MapGetRoadsFrom(m, agent->location, roads);

    // Filter out roads that the agent does not have enough stamina for
    int numLegalRoads = filterRoads(agent, roads, numRoads, legalRoads);

    struct move move;
    if (numLegalRoads > 0) {
        // nextMove is randomly chosen from the legal roads
        int k = rand() % numLegalRoads;
        move = (struct move){legalRoads[k].to, legalRoads[k].length};
    } else {
        // The agent must stay in the same location
        move = (struct move){agent->location, 0};
    }

    free(legalRoads);
    free(roads);
    return move;
}

static struct move chooseCheapestLeastVisited(Agent agent, Map m) {
    struct road *roads = malloc(MapNumCities(m) * sizeof(struct road));
    struct road *legalRoads = malloc(MapNumCities(m) * sizeof(struct road));

    int numRoads = MapGetRoadsFrom(m, agent->location, roads);
    int numLegalRoads = filterRoads(agent, roads, numRoads, legalRoads);

    // flags to find best city to move to
    int bestCity = -1;
    int minVisits = INT_MAX;
    int minStaminaCost = INT_MAX;
    
    for (int i = 0; i < numLegalRoads; i++) {
        int city = legalRoads[i].to;
        int staminaCost = legalRoads[i].length;
        int visitCount = getCityVisitCount(agent, city);
        
        if (visitCount < minVisits) {
            bestCity = city;
            minVisits = visitCount;
            minStaminaCost = staminaCost;
        } else if (visitCount == minVisits && staminaCost < minStaminaCost) {
            bestCity = city;
            minStaminaCost = staminaCost;
        } else if (visitCount == minVisits && staminaCost == minStaminaCost && city < bestCity) {
            bestCity = city;
            minStaminaCost = staminaCost;
        }
    }

    if (bestCity == -1) {
        incrementCityVisitCount(agent, agent->location);
        free(legalRoads);
        free(roads);
        return (struct move){agent->location, 0};
    }

    incrementCityVisitCount(agent, bestCity);
    struct move move = (struct move){bestCity, minStaminaCost};

    free(legalRoads);
    free(roads);
    
    return move;
}

static struct move chooseDFS(Agent agent, Map m) {
    // check if more cities to visit first
    if (agent->dfsPathIndex >= agent->dfsPathLength) {
        incrementCityVisitCount(agent, agent->location);
        return (struct move){agent->location, 0}; 
    }
        
    struct road *roads = malloc(MapNumCities(m) * sizeof(struct road));
    int numRoads = MapGetRoadsFrom(m, agent->location, roads);

    int nextCity = agent->dfsPath[agent->dfsPathIndex];
    struct road *nextRoad = NULL;

    // Find next road
    for (int i = 0; i < numRoads; i++) {
        if (roads[i].to == nextCity) {
            nextRoad = &roads[i];
            break;
        }
    }

    // Check if the next road is found 
    if (nextRoad != NULL && nextRoad->length <= agent->stamina) {
        agent->dfsPathIndex++; 
        incrementCityVisitCount(agent, nextCity); 
        struct move result = {nextCity, nextRoad->length};
        free(roads);
        return result; 
    } else { // Stay
        incrementCityVisitCount(agent, agent->location);
        agent->stamina = agent->maxStamina;  
        struct move result = (struct move){agent->location, 0};
        free(roads);
        return result;  
    } 
}

// Takes an array with all the possible roads and puts the ones the agent
// has enough stamina for into the legalRoads array
// Returns the number of legal roads
static int filterRoads(Agent agent, struct road roads[], int numRoads,
                       struct road legalRoads[]) {
    int numLegalRoads = 0;
    for (int i = 0; i < numRoads; i++) {
        if (roads[i].length <= agent->stamina) {
            legalRoads[numLegalRoads++] = roads[i];
        }
    }
    return numLegalRoads;
}

/**
 * Executes a given move by updating the agent's internal state
 */
void AgentMakeNextMove(Agent agent, struct move move) {
    if (move.to == agent->location) {
        agent->stamina = agent->maxStamina;
    } else {
        agent->stamina -= move.staminaCost;
    }
    agent->location = move.to;
    
    // TODO: You may need to add to this to handle different strategies

    if (agent->hasThiefLocation && agent->location == agent->thiefLocation) {
        // Agent has reached the thief's last known location
        agent->hasThiefLocation = false;
        free(agent->pathToThief);
        agent->pathToThief = NULL;
        agent->pathLength = 0;
        agent->pathIndex = 0;
    } else if (agent->hasThiefLocation && agent->pathIndex >= agent->pathLength) {
        // Agent has finished the path to the thief last known location
        agent->hasThiefLocation = false;
        free(agent->pathToThief);
        agent->pathToThief = NULL;
        agent->pathLength = 0;
        agent->pathIndex = 0;
    }
}

////////////////////////////////////////////////////////////////////////
// Learning information

/**
 * Tells the agent where the thief is
 */
void AgentTipOff(Agent agent, int thiefLocation) {
    // TODO: Stage 3
    agent->thiefLocation = thiefLocation;
    agent->hasThiefLocation = true;

    updatePathToThief(agent, agent->map);
}

////////////////////////////////////////////////////////////////////////
// Displaying state

/**
 * Prints information about the agent (for debugging purposes)
 */
void AgentShow(Agent agent) {
    // TODO: You can implement this function however you want
    //       You can leave this function blank if you want
}

////////////////////////////////////////////////////////////////////////
// Helper Functions

/**
 * Gets the amount of times an agent has visited a particular city
 */
static int getCityVisitCount(Agent agent, int city) {
    for (int i = 0; i < MapNumCities(agent->map); i++) {
        if (agent->timesVisited[i].cityIndex == city) {
            return agent->timesVisited[i].visitCount;
        }
    }
    return 0;
}

/**
 * Increments the cityVisit count
 */
static void incrementCityVisitCount(Agent agent, int cityIndex) {
    for (int i = 0; i < MapNumCities(agent->map); i++) {
        if (agent->timesVisited[i].cityIndex == cityIndex) {
            agent->timesVisited[i].visitCount++;
            return;
        }
    }
}

/**
 * Perform DFS 
 */
static void dfs(Map m, int currentCity, bool *visited, int *dfsPath, int *dfsPathIndex) {
    
    visited[currentCity] = true;
    dfsPath[*dfsPathIndex] = currentCity;
    (*dfsPathIndex)++;

    struct road *roads = malloc(MapNumCities(m) * sizeof(struct road));
    int numRoads = MapGetRoadsFrom(m, currentCity, roads);

    for (int i = 0; i < numRoads; i++) {
        int nextCity = roads[i].to;
        if (!visited[nextCity]) {
            dfs(m, nextCity, visited, dfsPath, dfsPathIndex);
        }
    }

    free(roads);  
}

/**
 * Initialize an Agents DFS path from their source city
 */
static void initializeDFSPath(Agent agent, Map m) {
    bool *visited = malloc(MapNumCities(m) * sizeof(bool));
    for (int i = 0; i < MapNumCities(m); i++) {
        visited[i] = false;
    }

    agent->dfsPath = malloc(MapNumCities(m) * sizeof(int));  
    agent->dfsPathIndex = 0;  
    agent->dfsPathLength = 0;  

    dfs(m, agent->startLocation, visited, agent->dfsPath, &agent->dfsPathIndex);

    agent->dfsPathLength = agent->dfsPathIndex;
    agent->dfsPathIndex = 1;

    free(visited);
}

/**
 * Updates the agents path to goto the present location of the thief
 */
static void updatePathToThief(Agent agent, Map m) {
    if (agent->hasThiefLocation) {
        // reset path to find shortest
        if (agent->pathToThief != NULL) {
            free(agent->pathToThief);
        }
        agent->pathToThief = dijkstra(agent, m, agent->location, agent->thiefLocation);
        agent->pathLength = 0;
    }
}

/**
 * Calculates the shortest path to the thief from the agents current position and returns
 * this into an array
 */
static int *dijkstra(Agent agent, Map m, int startCity, int targetCity) {
    int numCities = MapNumCities(m);
    int *dist = malloc(numCities * sizeof(int));  
    int *prev = malloc(numCities * sizeof(int));  
    Pq pq = PqNew();

    // Initialize distances to infinity and prev to -1 
    for (int i = 0; i < numCities; i++) {
        dist[i] = INT_MAX;
        prev[i] = -1;
    }
    dist[startCity] = 0;  

    struct road *roads = malloc(numCities * sizeof(struct road));
    int numRoads = MapGetRoadsFrom(m, startCity, roads);
    for (int i = 0; i < numRoads; i++) {
        PqInsert(pq, roads[i]);  
    }

    while (!PqIsEmpty(pq)) {
        struct road currentRoad = PqExtract(pq);
        int u = currentRoad.from;
        int v = currentRoad.to;
        int weight = currentRoad.length;

        // If shorter path exists, continue
        if (dist[v] <= dist[u] + weight) {
            continue;
        }

        dist[v] = dist[u] + weight;
        prev[v] = u;

        // Add the neighboring roads into priority queue
        int numNeighboringRoads = MapGetRoadsFrom(m, v, roads);
        for (int i = 0; i < numNeighboringRoads; i++) {
            PqInsert(pq, roads[i]);
        }
    }

    // Construct the path to the target city 
    int* path = malloc((numCities + 1) * sizeof(int)); 
    int pathIndex = 0;

    int city = targetCity;
    while (city != -1) {
        path[pathIndex++] = city;
        city = prev[city];
    }

    for (int i = 0; i < pathIndex / 2; i++) {
        int temp = path[i];
        path[i] = path[pathIndex - 1 - i];
        path[pathIndex - 1 - i] = temp;
    }
    path[pathIndex] = -1;

    free(dist);
    free(prev);
    free(roads);
    PqFree(pq);

    return path;
}

