#ifndef NAVIGATION_HPP
#define NAVIGATION_HPP

#include <stdio.h>      // for printf
#include <unistd.h>     // for usleep
#include <signal.h>     // for catching exit signals
#include <iostream>
#include <vector>

using std::vector;
using std::cout;
using std::cin;
using std::endl;

struct coordinates;
struct gridPoints;

vector<vector<bool>> makeGrid(gridPoints GP);
vector<vector<bool>> getGrid(gridPoints & GP);
void getCoordinates(gridPoints & GP, vector<vector<bool>> & grid);
void testFunctie(gridPoints GP, vector<vector<bool>> grid);
void moveToHomepoint(gridPoints GP);
void resetCurrentLocation(gridPoints & GP);
void updateLocation(gridPoints & GP, int distance);
void moveForward();
void turnLeft(gridPoints & GP);
void turnRight(gridPoints & GP);
void moveForwardDistance(gridPoints & GP, unsigned int distance);
void turn(char direction, gridPoints GP);


#endif /* NAVIGATION_HPP */
