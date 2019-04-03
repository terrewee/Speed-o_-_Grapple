#include "BrickPi3.h" // for BrickPi3
#include <stdio.h>      // for printf
#include <unistd.h>     // for usleep
#include <signal.h>     // for catching exit signals
#include <iostream>
#include <vector>

using std::vector;
using std::cout;
using std::cin;
using std::endl;

BrickPi3 BP;

struct coordinates{
  int x;
  int y;
};

//contains information on all needed grid coordinates, and facing direction of robot.
struct gridPoints{
  coordinates targetCoordinates;
  coordinates targetRelCoordinates;
  coordinates homeCoordinates;
  coordinates currentLocation;
  char direction;
};

void exit_signal_handler(int signo);

//Generates grid based on GP.targetRelCoordinates, padding levels can be adjusted with the + in the for loops.
vector<vector<bool>> makeGrid(gridPoints GP) {
	vector<vector<bool>> grid;
	int targetX = GP.targetRelCoordinates.x;
	int targetY = GP.targetRelCoordinates.y;
	if (targetX < 0) {
		targetX = targetX * -1;
	}
	if (targetY < 0) {
		targetY = targetY * -1;
	}
	for (size_t i = 0; i < targetY + 5; i++) {
		vector<bool> tempRow = {};
		for (size_t j = 0; j < targetX + 5; j++) {
			tempRow.push_back(true);
		}
		grid.push_back(tempRow);
	}
	return grid;
}

//Asks user for GP.targetRelCoordinates for grid generation.
vector<vector<bool>> getGrid(gridPoints & GP) {
	vector<vector<bool>> grid = { {} };

	cout << "Please give the relative x coordinate of the object to be found." << endl;
	cin >> GP.targetRelCoordinates.x;
	cout << "Please give the relative y coordinate of the object to be found." << endl;
	cin >> GP.targetRelCoordinates.y;

	grid = makeGrid(GP);

	return grid;
}

//Sets all the coordinates for GP.homeCoordinates and GP.targetCoordinates based on GP.targetRelCoordinates.
void getCoordinates(gridPoints & GP, vector<vector<bool>> & grid) {
	int ySize = grid.size();
	int xSize = grid[1].size();
	//Set home coordinates
	if (GP.targetRelCoordinates.x >= 0 && GP.targetRelCoordinates.y >= 0) {   //if X-Coordinate is + and Y-Coordinate is +
		GP.homeCoordinates.x = 2;
		GP.homeCoordinates.y = 2;
	}
	else if (GP.targetRelCoordinates.x < 0 && GP.targetRelCoordinates.y < 0) {  //if X-Coordinate is - and Y-Coordinate is -
		GP.homeCoordinates.x = xSize - 3;
		GP.homeCoordinates.y = ySize - 3;
	}
	else if (GP.targetRelCoordinates.x > 0 && GP.targetRelCoordinates.y < 0) {    //if X-Coordinate is + and Y-Coordinate is -
		GP.homeCoordinates.x = 2;
		GP.homeCoordinates.y = ySize - 3;
	}
	else {                                      //if X-Coordinate is - and Y-Coordinate is +
		GP.homeCoordinates.x = xSize - 3;
		GP.homeCoordinates.y = 2;
	}
	//Set target coordinates
	GP.targetCoordinates.x = GP.homeCoordinates.x + GP.targetRelCoordinates.x;
	GP.targetCoordinates.y = GP.homeCoordinates.y + GP.targetRelCoordinates.y;
}

//Prints grid for debugging and testing grid generation.
void testFunctie(gridPoints GP, vector<vector<bool>> grid) {
	for (size_t i = 0; i < grid.size(); i++) {
		for (size_t j = 0; j < grid[i].size(); j++) {
			if (GP.targetCoordinates.x == j && GP.targetCoordinates.y == i) {
				cout << 'T' << ' ';
			}
			else if (GP.homeCoordinates.x == j && GP.homeCoordinates.y == i) {
				cout << 'H' << ' ';
			}
			else {
				cout << grid[i][j] << ' ';
			}
		}
		cout << endl;
	}
}

//Moves robot forward one gridPoint.
void moveToHomepoint(){
  //move()
};

//Sets GP.currentCoordinates to GP.homeCoordinates (homepoint coordinates.)
void resetCurrentLocation(gridPoints & GP){
  GP.currentLocation.x = GP.homeCoordinates.x;
  GP.currentLocation.y = GP.homeCoordinates.y;
}

//Updates GP.currentCoordinates according to distance moved and GP.direction.
void updateLocation(gridPoints & GP, int distance){
  if(GP.direction == 'n'){
    GP.currentLocation.y -= distance;
  }
  else if(GP.direction == 's'){
    GP.currentLocation.y += distance;
  }
  else if if(GP.direction == 'w'){
    GP.currentLocation.x += distance;
  }
  else{
    GP.currentLocation.y -= distance;
  }
}

//Moves robot one grid unit forward, do NOT use this function to move the robot. moveForwardDistance() is made for that.
void moveForward(){
  
}

//Turns the rorbot to the right, and updates the value of GP.direction.
void turnLeft(gridPoints & GP){
  if(GP.direction == 'n'){
    GP.direction = 'e';
  }
  else if(GP.direction == 'w'){
    GP.direction = 'n';
  }
  else if(GP.direction == 's'){
    GP.direction = 'w';
  }
  else{
    GP.direction = 's';
  }
}

//Turns the rorbot to the left, and updates the value of GP.direction.
void turnRight(gridPoints & GP){
  if(GP.direction == 'n'){
    GP.direction = 'w';
  }
  else if(GP.direction == 'w'){
    GP.direction = 's';
  }
  else if(GP.direction == 's'){
    GP.direction = 'e';
  }
  else{
    GP.direction = 'n';
  }
}

//Moves robot a set distance forward and calls updateLocation().
void moveForwardDistance(gridPoints & GP, unsigned int distance){
  int count = 0;

  while(count < distance){
    moveForward();
    count++;
  }

  updateLocation(GP, int distance);
}

int main(){
  gridPoints GP;
  vector<vector<bool>> grid = getGrid(GP);
  getCoordinates(GP, grid);
  testFunctie(GP, grid);
  moveToHomepoint();
  resetCurrentLocation();
  return 0;
}

// Signal handler that will be called when Ctrl+C is pressed to stop the program
void exit_signal_handler(int signo){
  if(signo == SIGINT){
    BP.reset_all();    // Reset everything so there are no run-away motors
    exit(-2);
  }
}