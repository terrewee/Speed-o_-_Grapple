#ifndef NAVIGATION_HPP
#define NAVIGATION_HPP

struct coordinates;
struct girdPoints;

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
