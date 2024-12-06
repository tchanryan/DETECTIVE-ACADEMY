In this assignment, you are the police! You will control four detectives as they travel through a network of cities trying to catch a thief.

The police are aiming for any of the four detectives to catch the thief before the thief escapes to the getaway city, and before the time runs out... and the aim of the thief is to reach the getaway city before they are caught.

The detectives have a map, but do not know where the thief is or where they are trying to get to. The thief also has a map but unfortunately they didn't take COMP2521, so they don't really know how to use it and they wander randomly through the cities trying to reach the getaway city. The detectives may employ different strategies depending on what they have been assigned to.

Game Rules
In this game, all the people (the four detectives and the thief) are known as agents, and the game consists of a series of turns, known as cycles.

Each agent starts in a city, determined by user configuration. Every cycle, each agent may move from their current city to an adjacent city by road. The goal of the detectives is to end up in the same city as the thief, which would allow them to catch the thief, while the goal of the thief is to reach the getaway city.

Each agent begins with some stamina, also determined by user configuration. Whenever an agent moves from one city to another, they lose stamina equal to the length of the road between them.

Agents cannot travel along a road if they do not have the required level of stamina. This means it is possible for an agent to have no legal moves. If an agent has no legal moves due to not having enough stamina, they must remain in their current city for another cycle. Remaining in the same city for a turn resets the agent's stamina back to its initial level.

Each detective uses a set strategy to navigate the cities, determined by user configuration. Meanwhile, the thief always moves randomly.

The game ends if one of the following conditions is met:

If a detective starts in the same city as the thief, the thief is caught immediately and the detectives win.
If a detective is in the same city as the thief at the end of a turn, the thief is caught and the detectives win.
If the thief is in the getaway city at the end of a turn and there are no detectives there, the thief escapes, so the thief wins.
If the time has run out, regardless of whether the thief was able to reach the getaway city, the trail has gone cold, so the thief wins.
