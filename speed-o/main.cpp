#include "BrickPi3.h"	// for BrickPi3
#include "Navigation.h"	// for Navigation
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

void moveToHomepoint(gridPoints GP){
	if(GP.targetCoordinates.y == 0) && GP.targetCoordinates.x == 0){/*communicate();*/}
	turnLeft(GP);
  moveForwardDistance(GP);
	if(GP.targetCoordinates.y == 0){
		if		 (GP.targetCoordinates.x > 0){turnRight(GP);}
		else if(GP.targetCoordinates.x < 0){turnLeft(GP); turnLeft(GP);}
	}
	else if(GP.targetCoordinates.y > 0){turnRight(GP);}
	else if(GP.targetCoordinates.y < 0){turnLeft(GP);}


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
int turnMotorPowerUp(int motorPower) {
	while (motorPower < 60) {
		BP.set_motor_power(PORT_B, motorPower);
		BP.set_motor_power(PORT_C, motorPower);
		motorPower += 5;
		usleep(0.1);
	}
	return motorPower;
}

void turnMotorPowerDown(int motorPower) {
	while (motorPower > 10) {
		BP.set_motor_power(PORT_B, motorPower);
		BP.set_motor_power(PORT_C, motorPower);
		motorPower -= 5;
		usleep(0.1);
	}
}

void moveForward(){
	int motorPower = 10;
	turnMotorPowerUp(motorPower);
	sleep(1);
	turnMotorPowerDown(motorPower);
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

// Tells the robot which way to turn.
void turn(char direction, gridPoints GP) {
	if (GP.currentLocation == 'n') {
		if (direction == 'w') {
			turnLeft();
		}
		else if (direction == 's') {
			turnLeft();
			turnLeft();
		}
		else if (direction == 'e') {
			turnRight();
		}
	}
	else if (GP.currentLocation == 'e') {
		if (direction == 'w') {
			turnLeft();
			turnLeft();
		}
		else if (direction == 's') {
			turnRight();
		}
		else if (direction == 'n') {
			turnLeft();
		}
	}
	else if (GP.currentLocation == 's') {
		if (direction == 'e') {
			turnLeft();
		}
		else if (direction == 'n') {
			turnLeft();
			turnLeft();
		}
		else if (direction == 'w') {
			turnRight();
		}
	}
	else if (GP.currentLocation == 'w') {
		if (direction == 's') {
			turnLeft();
		}
		else if (direction == 'e') {
			turnLeft();
			turnLeft();
		}
		else if (direction == 'n') {
			turnRight();
		}
	}
}

//updates the current coordinate in prevCoordinatesVector with previous coordinates.
void updatePrevCoordinates(coordinates & currentCoordinates, coordinates & prevCoordinates, vector<vector<int>> & prevCoordinatesVector, vector<vector<bool>> & grid){
	unsigned int columnAmount = grid.size();
	unsigned int rowAmount = grid[0].size();
	vector<int> gridPointVector[2];
	
	gridPointVector[0] = prevCoordinates.x;
	gridPointVector[1] = prevCoordinates.y;

	prevCoordinatesVector[currentCoordinates.x][currentCoordinates.y] = gridPointVector;
}

//Gets the coordinates of a gridPoint from its number.
coordinates getGridPointCoordinates(int number, vector<vector<bool>> & grid){
	unsigned int columnAmount = grid.size();
	unsigned int rowAmount = grid[0].size();
	unsigned int gridsize = rowAmount * columnAmount;
	coordinates gridPointCoordinates;

	if(number < rowAmount){
		gridPointCoordinates.x = number;
		gridPointCoordinates.y = 0;
	}
	else{
		gridPointCoordinates.x = number % rowAmount;
		gridPointCoordinates.y = number / columnAmount;
	}

	return gridPoint;
	
}

//Gets the number of a gridPoint from coordinates.
int getGridPointNumber(coordinates & gridPoint, vector<vector<bool>> & grid){
	unsigned int columnAmount = grid.size();
	unsigned int rowAmount = grid[0].size();
	unsigned int gridsize = rowAmount * columnAmount;
	int gridPointNumber;

	gridPointNumber = gridPoint.x + (gridPoint.y *rowAmount);

	return gridPointNumber;
}

//Check bordering gridpoints and put them in a list if they are on grid.
vector<int> checkOptions(coordinates gridPoint, <vector<vector<bool> grid){
	vector<int> queue;
	
	coordinates optionA;
	optionA.x = gridPoint.x - 1;
	optionA.y = gridPoint.y;
	
	if(checkInGrid(optionA, grid) == 1){queue.pushback(getGridPointNumber(optionA));}

	coordinates optionB;
	optionB.x = gridPoint.x;
	optionB.y = gridPoint.y - 1;

	if(checkInGrid(optionB, grid) == 1){queue.pushback(getGridPointNumber(optionB));}
	
	coordinates optionC;
	optionC.x = gridPoint.x + 1;
	optionC.y = gridPoint.y;

	if(checkInGrid(optionC, grid) == 1){queue.pushback(getGridPointNumber(optionC));}

	coordinates optionD;
	optionD.x = gridPoint.x;
	optionD.y = gridPoint.y + 1;

	if(checkInGrid(optionD, grid) == 1){queue.pushback(getGridPointNumber(optionD));}

	return queue;
}

//Check if point is on the grid.
bool checkInGrid(coordinates pathCheck, vector<vector<bool>> grid){
	if			(pathCheck.x < 0)									{return 0;}
	else if	(pathCheck.x > grid[0].size()-1)	{return 0;}
	else if	(pathCheck.y < 0)									{return 0;}
	else if	(pathCheck.y > grid.size()-1)			{return 0;}
	else 																			{return 1;}
}

//Check if grid point is end point.
bool checkIfTarget(coordinates targetCheck, gridPoints GP){
	if(GP.targetRelCoordinates.x == targetCheck.x && GP.targetRelCoordinates.y == targetCheck.y){return 1;}
	else {return 0;}
}

int main(){
	BP.detect();	//Make sure that the BrickPi3 is communicating and that the filmware is compatible with the drivers/

	//Reset the encoders
	BP.offset_motor_encoder(PORT_A, BP.get_motor_encoder(PORT_A));
	BP.offset_motor_encoder(PORT_B, BP.get_motor_encoder(PORT_B));
	BP.offset_motor_encoder(PORT_C, BP.get_motor_encoder(PORT_C));
	BP.offset_motor_encoder(PORT_D, BP.get_motor_encoder(PORT_D));

	// Read the encoders
	int32_t EncoderA = BP.get_motor_encoder(PORT_A);
	int32_t EncoderB = BP.get_motor_encoder(PORT_B);
	int32_t EncoderC = BP.get_motor_encoder(PORT_C);
	int32_t EncoderD = BP.get_motor_encoder(PORT_D);

	// Use the encoder value from motor A to control motors B, C, and D
	BP.set_motor_power(PORT_B, EncoderA < 100 ? EncoderA > -100 ? EncoderA : -100 : 100);
	BP.set_motor_dps(PORT_C, EncoderA);
	BP.set_motor_position(PORT_D, EncoderA);

	gridPoints GP;
	vector<vector<bool>> grid = getGrid(GP);
	getCoordinates(GP, grid);
	testFunctie(GP, grid);
	moveToHomepoint(GP);
	resetCurrentLocation();

	moveForward();

	return 0;
}

// Signal handler that will be called when Ctrl+C is pressed to stop the program
void exit_signal_handler(int signo){
  if(signo == SIGINT){
    BP.reset_all();    // Reset everything so there are no run-away motors
    exit(-2);
  }
}
