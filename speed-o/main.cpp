//#include "BrickPi3.h"	// for BrickPi3
//#include "Navigation.h"	// for Navigation
#include <stdio.h>      // for printf
#include <unistd.h>     // for usleep
#include <signal.h>     // for catching exit signals
#include <iostream>
#include <vector>
#include <string>

using std::vector;
using std::cout;
using std::cin;
using std::endl;
using std::string;

//BrickPi3 BP;

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
    //BP.reset_all();    // Reset everything so there are no run-away motors
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
	for (int i = 0; i < targetY + 5; i++) {
		vector<string> tempRow = {};
		for (int j = 0; j < targetX + 5; j++) {
			tempRow.push_back(".");
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
void getCoordinates(gridPoints &GP, vector<vector<bool>> &grid) {
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


//Moves robot one grid unit forward, do NOT use this function to move the robot. moveForwardDistance() is made for that.
void turnMotorPowerUp(int &motorPower) {
	int snelheid = 1;

	while (motorPower < snelheid) {
		//BP.set_motor_power(PORT_A, motorPower);
		//BP.set_motor_power(PORT_B, motorPower);
		usleep(0.5);
		motorPower += 1;

	}
}

void turnMotorPowerDown(int &motorPower) {
	while (motorPower > 10) {
		//BP.set_motor_power(PORT_A, motorPower+1);
		//BP.set_motor_power(PORT_B, motorPower);
		motorPower -= 10;
	}
}
void moveForward(gridPoints &GP){
	int motorPower = 10;
	turnMotorPowerUp(motorPower);
	sleep(1);
	turnMotorPowerDown(motorPower);
}

//Turns the rorbot to the right, and updates the value of GP.direction.
void turnLeft(gridPoints &GP){
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

//Turns the rorbot to the left, and updates the value of GP.direction.
void turnRight(gridPoints &GP){
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

//Sets GP.currentCoordinates to GP.homeCoordinates (homepoint coordinates.)
void resetCurrentLocation(gridPoints &GP){
  GP.currentLocation.x = GP.homeCoordinates.x;
  GP.currentLocation.y = GP.homeCoordinates.y;
}

//Updates GP.currentCoordinates according to distance moved and GP.direction.
void updateLocation(gridPoints &GP, const unsigned int distance){
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
void moveForwardDistance(gridPoints &GP, unsigned int distance){
  unsigned int count = 0;

  while(count < distance){
    moveForward(GP);
    count++;
  }

  updateLocation(GP, distance);
}

void moveToHomepoint(gridPoints &GP){
	GP.direction = 'n';
	if(GP.targetCoordinates.y == 0 && GP.targetCoordinates.x == 0){/*communicate();*/}
	turnLeft(GP);
  moveForwardDistance(GP, 1);
	if(GP.targetCoordinates.y == 0){
		if		 (GP.targetCoordinates.x > 0){turnRight(GP);}
		else if(GP.targetCoordinates.x < 0){turnLeft(GP); turnLeft(GP);}
	}
	else if(GP.targetCoordinates.y > 0){turnRight(GP);}
	else if(GP.targetCoordinates.y < 0){turnLeft(GP);}


}

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

string manualControl(gridPoints &GP){
	vector<char> orientationList;
	string answer;
	while(true){
		cin >> answer;
		if 			(answer == "w")		{moveForward(GP);}
		else if (answer == "a")		{turnLeft(GP); moveForward(GP);}
		else if (answer == "d")		{turnRight(GP); moveForward(GP);}
		else if (answer == "esc")	{break;}
		else 											{cout << "invalid input." << endl; continue;}
		orientationList.push_back(GP.direction);
		cout << GP.direction << endl << endl;
	}
	string message(orientationList.begin(), orientationList.end()); 
	return message;

}

int main(){
	/*
	signal(SIGINT, exit_signal_handler);
	BP.detect();	//Make sure that the BrickPi3 is communicating and that the filmware is compatible with the drivers/

	Reset the encoders
	BP.offset_motor_encoder(PORT_A, BP.get_motor_encoder(PORT_A));
	BP.offset_motor_encoder(PORT_B, BP.get_motor_encoder(PORT_B));
	BP.offset_motor_encoder(PORT_C, BP.get_motor_encoder(PORT_C));
	BP.offset_motor_encoder(PORT_D, BP.get_motor_encoder(PORT_D));

	Read the encoders
	int32_t EncoderA = BP.get_motor_encoder(PORT_A);
	int32_t EncoderB = BP.get_motor_encoder(PORT_B);
	int32_t EncoderC = BP.get_motor_encoder(PORT_C);
	int32_t EncoderD = BP.get_motor_encoder(PORT_D);

	Use the encoder value from motor A to control motors B, C, and D
	BP.set_motor_power(PORT_B, EncoderA < 100 ? EncoderA > -100 ? EncoderA : -100 : 100);
	BP.set_motor_dps(PORT_C, EncoderA);
	BP.set_motor_position(PORT_D, EncoderA);
	*/
	gridPoints GP;
	vector<vector<bool>> grid = getGrid(GP);
	getCoordinates(GP, grid);
	moveToHomepoint(GP);
	resetCurrentLocation(GP);

	string NOSWList = manualControl(GP);

	cout << NOSWList << " ";
		 
	//moveForward();
	cout << "end of file";
	return 0;
}