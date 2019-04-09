#include "BrickPi3.h"	// for BrickPi3
//#include "Navigation.h"	// for Navigation
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

// Signal handler that will be called when Ctrl+C is pressed to stop the program
void exit_signal_handler(int signo){
  if(signo == SIGINT){
    BP.reset_all();    // Reset everything so there are no run-away motors
    exit(-2);
  }
}


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
void resetMotorsAB(){
	BP.set_motor_power(PORT_A, 0);
	BP.set_motor_power(PORT_B, 0);
}
//Moves robot one grid unit forward, do NOT use this function to move the robot. moveForwardDistance() is made for that.
void turnMotorPowerUp(int &motorPower) {
	int snelheid =30;
	BP.offset_motor_encoder(PORT_A, BP.get_motor_encoder(PORT_A));
	BP.offset_motor_encoder(PORT_B, BP.get_motor_encoder(PORT_B));
	while (motorPower <= snelheid) {
		BP.set_motor_power(PORT_A, motorPower+6);
		BP.set_motor_power(PORT_B, motorPower);
		int32_t EncoderA = BP.get_motor_encoder(PORT_A);
		int32_t EncoderB = BP.get_motor_encoder(PORT_B);
		int32_t EncoderC = BP.get_motor_encoder(PORT_C);
		int32_t EncoderD = BP.get_motor_encoder(PORT_D);
		printf("Encoder A: %6d  B: %6d  C: %6d  D: %6d\n", EncoderA, EncoderB, EncoderC, EncoderD);
		motorPower += 5;
	}
}

void moveForward(){
	int motorPower = 10;
	turnMotorPowerUp(motorPower);
	sleep(1);
	resetMotorsAB();
}

//Turns the robot to the right, and updates the value of GP.direction.
void turnLeft(gridPoints & GP){
  int lpos = -88; 		//Left position
  int rpos = 88;		//Right position
  if(GP.direction == 'n'){
    GP.direction = 'e';
	//Port A -175 Port B 175
		BP.set_motor_position_relative(PORT_A, lpos);
		BP.set_motor_position_relative(PORT_B, rpos);
		sleep(1);
		resetMotorsAB();
  }
  else if(GP.direction == 'w'){
    GP.direction = 'n';
		BP.set_motor_position_relative(PORT_A, lpos);
		BP.set_motor_position_relative(PORT_B, rpos);
		sleep(1);
		resetMotorsAB();
  }
  else if(GP.direction == 's'){
    GP.direction = 'w';
		BP.set_motor_position_relative(PORT_A, lpos);
		BP.set_motor_position_relative(PORT_B, rpos);
		sleep(1);
		resetMotorsAB();
  }
  else{
    GP.direction = 's';
		BP.set_motor_position_relative(PORT_A, lpos);
		BP.set_motor_position_relative(PORT_B, rpos);
		sleep(1);
		resetMotorsAB();
  }
}

//Turns the rorbot to the left, and updates the value of GP.direction.
void turnRight(gridPoints & GP){
  int lpos = 88;		//Left position
  int rpos = -88;		//Right position	
  if(GP.direction == 'n'){
    GP.direction = 'w';
		BP.set_motor_position_relative(PORT_A, lpos);
		BP.set_motor_position_relative(PORT_B, rpos);
		sleep(1);
		resetMotorsAB();
  }
  else if(GP.direction == 'w'){
    GP.direction = 's';
		BP.set_motor_position_relative(PORT_A, lpos);
		BP.set_motor_position_relative(PORT_B, rpos);
		sleep(1);
		resetMotorsAB();
  }
  else if(GP.direction == 's'){
    GP.direction = 'e';
		BP.set_motor_position_relative(PORT_A, lpos);
		BP.set_motor_position_relative(PORT_B, rpos);
		sleep(1);
		resetMotorsAB();
  }
  else{
    GP.direction = 'n';
		BP.set_motor_position_relative(PORT_A, lpos);
		BP.set_motor_position_relative(PORT_B, rpos);
		sleep(1);
		resetMotorsAB();
  }
}

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
  else if(GP.direction == 'w'){
    GP.currentLocation.x -= distance;
  }
  else{
    GP.currentLocation.y += distance;
  }
}

//Moves robot a set distance forward and calls updateLocation().
void moveForwardDistance(gridPoints & GP, unsigned int distance){
  int count = 0;

  while(count < distance){
    moveForward();
    count++;
  }

  updateLocation(GP, distance);
}

void moveToHomepoint(gridPoints GP){
	if(GP.targetCoordinates.y == 0 && GP.targetCoordinates.x == 0){/*communicate();*/}
	turnLeft(GP);
  moveForwardDistance(GP, 1);
	if(GP.targetCoordinates.y == 0){
		if		 (GP.targetCoordinates.x > 0){turnRight(GP);}
		else if(GP.targetCoordinates.x < 0){turnLeft(GP); turnLeft(GP);}
	}
	else if(GP.targetCoordinates.y > 0){turnRight(GP);}
	else if(GP.targetCoordinates.y < 0){turnLeft(GP);}


};

// Tells the robot which way to turn.
void turn(char direction, gridPoints GP) {
	if (GP.direction == 'n') {
		if (direction == 'w') {
			turnLeft(GP);
		}
		else if (direction == 's') {
			turnLeft(GP);
			turnLeft(GP);
		}
		else if (direction == 'e') {
			turnRight(GP);
		}
	}
	else if (GP.direction == 'e') {
		if (direction == 'w') {
			turnLeft(GP);
			turnLeft(GP);
		}
		else if (direction == 's') {
			turnRight(GP);
		}
		else if (direction == 'n') {
			turnLeft(GP);
		}
	}
	else if (GP.direction == 's') {
		if (direction == 'e') {
			turnLeft(GP);
		}
		else if (direction == 'n') {
			turnLeft(GP);
			turnLeft(GP);
		}
		else if (direction == 'w') {
			turnRight(GP);
		}
	}
	else if (GP.direction == 'w') {
		if (direction == 's') {
			turnLeft(GP);
		}
		else if (direction == 'e') {
			turnLeft(GP);
			turnLeft(GP);
		}
		else if (direction == 'n') {
			turnRight(GP);
		}
	}
}

//updates the current coordinate in prevCoordinatesVector with previous coordinates.
void updatePrevCoordinates(coordinates & currentCoordinates, coordinates & prevCoordinates, vector<vector<int>> & prevCoordinatesVector, vector<vector<bool>> & grid){
	unsigned int columnAmount = grid.size();
	unsigned int rowAmount = grid[0].size();
	vector<int> gridPointVector;
	
	gridPointVector[0] = prevCoordinates.x;
	gridPointVector[1] = prevCoordinates.y;

	//prevCoordinatesVector[currentCoordinates.x][currentCoordinates.y] = gridPointVector;
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

	return gridPointCoordinates;
	
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

void addToQueue(coordinates & option, coordinates & gridPoint, vector<vector<int>> & prevCoordinatesVector, vector<vector<bool>> & grid, vector<int> & queue){
	bool optionFound = false;
	int gridPointNumber = getGridPointNumber(option, grid);

	for(unsigned int i = 0; i < queue.size(); i++){
		if(queue[i] == gridPointNumber){
			optionFound = true;
		}
	}

	if(!optionFound){
		queue.push_back(gridPointNumber);
		updatePrevCoordinates(option, gridPoint, prevCoordinatesVector, grid);
	}
}

//Check if point is on the grid.
bool checkInGrid(coordinates pathCheck, vector<vector<bool>> & grid){
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

//Check bordering gridpoints and calls addToQueue if they are on grid.
vector<int> updateQueue(int gridPointNumber, vector<vector<int>> prevCoordinatesVector, vector<vector<bool>> grid){
	vector<int> queue;
	
	coordinates gridPoint = getGridPointCoordinates(gridPointNumber, grid);
	
	coordinates optionA;
	optionA.x = gridPoint.x - 1;
	optionA.y = gridPoint.y;
	if(checkInGrid(optionA, grid) == 1){addToQueue(optionA, gridPoint, prevCoordinatesVector, grid, queue);}

	coordinates optionB;
	optionB.x = gridPoint.x;
	optionB.y = gridPoint.y - 1;
	if(checkInGrid(optionB, grid) == 1){addToQueue(optionB, gridPoint, prevCoordinatesVector, grid, queue);}
	
	coordinates optionC;
	optionC.x = gridPoint.x + 1;
	optionC.y = gridPoint.y;
	if(checkInGrid(optionC, grid) == 1){addToQueue(optionC, gridPoint, prevCoordinatesVector, grid, queue);}

	coordinates optionD;
	optionD.x = gridPoint.x;
	optionD.y = gridPoint.y + 1;
	if(checkInGrid(optionD, grid) == 1){addToQueue(optionD, gridPoint, prevCoordinatesVector, grid, queue);}

	return queue;
}

void searchPath(gridPoints & GP, vector<vector<bool>> & grid){
	bool targetFound = false;
	int homeGridPointNumber = getGridPointNumber(GP.homeCoordinates, grid);
	vector<vector<int>> prevCoordinatesVector(grid.size() * grid[0].size());
	vector<int> queue = updateQueue(homeGridPointNumber, prevCoordinatesVector, grid);
	unsigned int i = 1;

	while(!targetFound && queue[queue.size() -1] != (grid.size() * grid[0].size() -1)){
		updateQueue(queue[i], prevCoordinatesVector, grid);

		for(unsigned int i = 0; i < queue.size(); i++){
			coordinates gridPoint = getGridPointCoordinates(queue[i], grid);
			if(gridPoint.x == GP.homeCoordinates.x && gridPoint.y == GP.homeCoordinates.y){
				targetFound == true;
			}
		}
		i++;
	}

}

int main(){
	signal(SIGINT, exit_signal_handler);
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
	// testFunctie(GP, grid);
	// moveToHomepoint(GP);
	// resetCurrentLocation(GP);

	moveForward();
	// turnLeft(GP);
	// turnRight(GP);
	printf("Encoder A: %6d  B: %6d  C: %6d  D: %6d\n", EncoderA, EncoderB, EncoderC, EncoderD);

	return 0;
}