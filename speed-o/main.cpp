#include "BrickPi3.h"     // for BrickPi3
#include <stdio.h>        // for printf
#include <unistd.h>       // for usleep
#include <signal.h>       // for catching exit signals
#include <stdlib.h>
#include <sys/socket.h>   //include voor het gebruik van sockets
#include <netinet/in.h>
#include <netdb.h>
#include <string>
#include <iostream>
#include <thread>
#include <vector>

using namespace std;

//------------------------------------------ESSENSIALS-----------------------------------------------

BrickPi3 BP;

bool battery = true;          //battery level function
bool running = 1;

void batteryLevel(){
  //printf("Battery voltage : %.3f\n", BP.get_voltage_battery());
  while(::running){
    if(BP.get_voltage_battery() <= 9.0){
      cout << "Battery be dead m8." << endl;
      ::battery = false;
    }
    else{
      ::battery = true;
    }
    sleep(5);
	}
}

// Signal handler that will be called when Ctrl+C is pressed to stop the program
void exit_signal_handler(int signo){
  if(signo == SIGINT){
		running = 0;
    BP.reset_all();    // Reset everything so there are no run-away motors
    exit(-2);
  }
}

void checkSensor(){
  BP.set_sensor_type(PORT_1,SENSOR_TYPE_NXT_ULTRASONIC);
  BP.set_sensor_type(PORT_2,SENSOR_TYPE_NXT_COLOR_FULL);
  BP.set_sensor_type(PORT_3,SENSOR_TYPE_NXT_LIGHT_ON);
  BP.set_sensor_type(PORT_4,SENSOR_TYPE_NXT_COLOR_FULL);
}

//------------------------------------------CONNECTION-----------------------------------------------

int ComPortNr = 6969;         //Port number for communication
char ComHostName[] = "dex2";  //Hostname for communication

void SetComm(){
  cout << endl << "Geef het poort-nummer op: ";
  cin >> ::ComPortNr; cout << endl;
  cout << endl << "Geef de host-name op: ";
  cin >> ::ComHostName; cout << endl;
}

void error(const char *msg) {
  perror(msg);
  exit(1);
}

void iClient(char message[256]){
  //zet de connectie op voor het verzenden van een message.
  char buffer[256];
  int socketFD, n;
  struct sockaddr_in serv_addr;
  struct hostent *server;

  socketFD = socket(AF_INET, SOCK_STREAM, 0);
  server = gethostbyname(::ComHostName);
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;

  bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr, server->h_length);
  serv_addr.sin_port = htons(::ComPortNr);

  if (connect(socketFD,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
    error("ERROR connecting");
  }

  bzero(buffer,256);
  bcopy(message,buffer,strlen(message));
  n = write(socketFD,buffer,strlen(buffer));

  if (n < 0){
    error("ERROR writing to socket");
  }

  bzero(buffer,256);
  n = read(socketFD,buffer,255);

  if (n < 0){
    error("ERROR reading from socket");
  }
  //Receive message "1" if sent message was received.
  printf("%s\n",buffer);
  close(socketFD);
}


//------------------------------------------MANUAL CONTROLS-----------------------------------------------


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

struct range {
  bool obstakelInRangeLeft = false;
  bool obstakelInRangeRight = false;
  bool obstakelInRangeForward = false;
};

range obstakel;



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
		vector<bool> tempRow = {};
		for (int j = 0; j < targetX + 5; j++) {
			tempRow.push_back(1);
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

//--------movement---------

//Moves robot one grid unit forward, do NOT use this function to move the robot. moveForwardDistance() is made for that.
void turnMotorPowerUp(int &motorPower) {
	int snelheid = 10;
	while (motorPower < snelheid) {
		BP.set_motor_power(PORT_A, motorPower);
		BP.set_motor_power(PORT_B, motorPower);
		usleep(0.5);
		motorPower += 1;

	}
}

void turnMotorPowerDown(int &motorPower) {
	while (motorPower > 10) {
		BP.set_motor_power(PORT_A, motorPower+1);
		BP.set_motor_power(PORT_B, motorPower);
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

//-------path instructions--------

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
    GP.currentLocation.x += distance;
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
void turn(char direction, gridPoints & GP) {
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


//------------------------------------------GRID-----------------------------------------------

void move(char direction, gridPoints & GP){
	turn(direction, GP);
	moveForwardDistance(GP, 1);
}

//Gets the coordinates of a gridPoint from its number.
coordinates getGridPointCoordinates(unsigned int number, vector<vector<bool>> & grid){
	unsigned int columnAmount = grid.size();
	unsigned int rowAmount = grid[0].size();
	//unsigned int gridsize = rowAmount * columnAmount;
	coordinates gridPointCoordinates;

	if(number < rowAmount){
		gridPointCoordinates.x = number;
		gridPointCoordinates.y = 0;
	}
	else{
		gridPointCoordinates.x = number % rowAmount;
		gridPointCoordinates.y = number / rowAmount;

	}

	return gridPointCoordinates;
	
}

//Gets the number of a gridPoint from coordinates.
int getGridPointNumber(coordinates & gridPoint, vector<vector<bool>> & grid){
	unsigned int rowAmount = grid[0].size();
	//unsigned int gridsize = rowAmount * columnAmount;
	return gridPoint.x + (gridPoint.y * rowAmount);
}

//updates the current coordinate in prevCoordinatesVector with previous coordinates.
void updatePrevCoordinates(coordinates & currentCoordinates, coordinates & prevCoordinates, vector<coordinates> & prevCoordinatesVector, vector<vector<bool>> & grid){
	prevCoordinatesVector[getGridPointNumber(currentCoordinates, grid)] = prevCoordinates;
}

void addToQueue(coordinates & option, coordinates & gridPoint, vector<coordinates> & prevCoordinatesVector, vector<vector<bool>> & grid, vector<int> & queue){
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
bool checkInGrid(coordinates pathCheck, vector<vector<bool>> &grid){
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
vector<int> updateQueue(int gridPointNumber, vector<coordinates> &prevCoordinatesVector, vector<int> queue, vector<vector<bool>> &grid){
	
	coordinates gridPoint = getGridPointCoordinates(gridPointNumber, grid);
	// cout << gridPoint.x << " " << gridPoint.y << " ; ";	
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

//Creates vector<coordinates> containing the coordinates of the route going from the target to the currenLocation.
void getRoute(vector<coordinates> & route, vector<coordinates> & prevCoordinatesVector, gridPoints & GP, vector<vector<bool>> & grid){
	unsigned int i = 1;
	route.push_back(GP.targetCoordinates);
	
	while(true){
		unsigned int nextPointNumber = getGridPointNumber(route[i - 1], grid);
		coordinates nextPointCoordinates = prevCoordinatesVector[nextPointNumber];
		route.push_back(nextPointCoordinates);
		i++;
		if((nextPointCoordinates.x == GP.currentLocation.x && nextPointCoordinates.y == GP.currentLocation.y) || i >= grid.size() * grid[0].size()){
			break;
		}
	}
}

//Creates string with directions going from currentLocation to target.
void getDirections(string & directions, vector<coordinates> & route, gridPoints & GP){
	for(unsigned int i = route.size(); i > 0; i--){
		if(route[i - 1].x == route[i].x && route[i - 1].y == route[i].y - 1){
			directions += 'n';
		}
		else if(route[i - 1].x == route[i].x && route[i - 1].y == route[i].y + 1){
			directions += 's';
		}
		else if(route[i - 1].x == route[i].x - 1 && route[i - 1].y == route[i].y){
			directions += 'w';
		}
		else if(route[i - 1].x == route[i].x + 1 && route[i - 1].y == route[i].y){
			directions += 'e';
		}
	}
}

//Dees everything with finding shortest route.
void searchPath(string & directions, gridPoints & GP, vector<vector<bool>> & grid){
	bool targetFound = false;
	int homeGridPointNumber = getGridPointNumber(GP.homeCoordinates, grid);
	vector<coordinates> prevCoordinatesVector(grid.size() * grid[0].size());
	vector<coordinates> route = {};
	vector<int> queue = {};
	queue = updateQueue(homeGridPointNumber, prevCoordinatesVector, queue, grid);
	unsigned int i = 1;
	cout << "test2" << endl;
	while(!targetFound && i < (grid.size() * grid[0].size()) -1){
		queue = updateQueue(queue[i], prevCoordinatesVector, queue, grid);
		unsigned int queueSize = queue.size();
		for(unsigned int j = 0; j < queueSize; j++){
			coordinates gridPoint = getGridPointCoordinates(queue[i], grid);
			if(gridPoint.x == GP.targetCoordinates.x && gridPoint.y == GP.targetCoordinates.y){
				targetFound = true;
			}
		}
		i++;
	}

	getRoute(route, prevCoordinatesVector, GP, grid);
	cout << "route:" << endl;
	for(unsigned int i = 0; i < route.size(); i++){
		cout << route[i].x << "," << route[i].y << " ";
	}

	directions = "";
	getDirections(directions, route, GP);
	cout << endl << "Directions:" << endl;
	cout << directions;
	cout << endl << "prevCoordinates:" << endl;
	for(size_t i = 0; i < prevCoordinatesVector.size(); i++){cout << prevCoordinatesVector[i].x << "," << prevCoordinatesVector[i].y << " ";}	
	cout << endl << endl;
}

void followRoute(string & followedRoute, bool & destinationArrived, gridPoints & GP, vector<vector<bool>> grid, range & obstacles){
	string directions;
	bool obstructed = false;

	searchPath(directions, GP, grid);

	while(!destinationArrived){
		if(obstructed){
			searchPath(directions, GP, grid);
			obstructed = false;
		}
		cout << "test1" << endl;
		for(int i = 0; i < directions.size(); i++){
			cout << i << "  " << directions[i] << ":";
			cout << GP.currentLocation.x << "," << GP.currentLocation.y << ";" << GP.direction << "|";
			if(obstacles.obstakelInRangeForward && directions[i] == GP.direction){																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																							
				if(GP.direction == 'n'){
					grid[GP.currentLocation.x][GP.currentLocation.y - 1] = 0;
					if(obstacles.obstakelInRangeLeft){
						grid[GP.currentLocation.x - 1][GP.currentLocation.y] = 0;
					}
					else if(obstacles.obstakelInRangeLeft){
						grid[GP.currentLocation.x + 1][GP.currentLocation.y] = 0;
					}
				}
				else if(GP.direction == 'e'){
					grid[GP.currentLocation.x + 1][GP.currentLocation.y] = 0;
					if(obstacles.obstakelInRangeLeft){
						grid[GP.currentLocation.x][GP.currentLocation.y - 1] = 0;
					}
					else if(obstacles.obstakelInRangeLeft){
						grid[GP.currentLocation.x][GP.currentLocation.y + 1] = 0;
					}
				}
				else if(GP.direction == 's'){
					grid[GP.currentLocation.x][GP.currentLocation.y + 1] = 0;
					if(obstacles.obstakelInRangeLeft){
						grid[GP.currentLocation.x + 1][GP.currentLocation.y] = 0;
					}
					else if(obstacles.obstakelInRangeLeft){
						grid[GP.currentLocation.x - 1][GP.currentLocation.y] = 0;
					}
				}
				else if(GP.direction == 'w'){
					grid[GP.currentLocation.x - 1][GP.currentLocation.y] = 0;
					if(obstacles.obstakelInRangeLeft){
						grid[GP.currentLocation.x][GP.currentLocation.y + 1] = 0;
					}
					else if(obstacles.obstakelInRangeLeft){
						grid[GP.currentLocation.x][GP.currentLocation.y - 1] = 0;
					}
				}

				obstructed = true;
			}
			else{
				move(directions[i], GP);
				followedRoute += directions[i];

				if(i == directions.size() - 1){
					destinationArrived = true;
				}
			}

			cout << i << "  " << directions[i] << ":";
			cout << GP.currentLocation.x << "," << GP.currentLocation.y << ";" << GP.direction << "|";
		}
		
	}
}

void dockScout(gridPoints & GP){
	move('e', GP);
	turn('n', GP);
}

void driveBack(string followedRoute, gridPoints & GP){
	char tempChar;
	for(unsigned int i = 0; i < followedRoute.size() / 2; i++){
		tempChar = followedRoute[followedRoute.size() - (i + 1)];
		followedRoute[followedRoute.size() - (i + 1)] = followedRoute[i];
		followedRoute[i] = tempChar;
	}
	for(unsigned int k = 0; k < followedRoute.size(); k++){
		if(followedRoute[k] == 'n'){
			followedRoute[k] = 's';
		}
		else if(followedRoute[k] == 'e'){
			followedRoute[k] = 'w';
		}
		else if(followedRoute[k] == 's'){
			followedRoute[k] = 'n';
		}
		else if(followedRoute[k] == 'w'){
			followedRoute[k] = 'e';
		}
	}

	//debugging
	cout << "route back:" << endl;
	for(unsigned int i = 0; i < followedRoute.size(); i++){
		cout << followedRoute[i];
	}
	cout<< endl;

	for(unsigned int j = 0; j < followedRoute.size(); j++){
		cout << j << "  " << followedRoute[j] << ":";
		cout << GP.currentLocation.x << "," << GP.currentLocation.y << ";" << GP.direction << "|";

		if(followedRoute[j] == 'n'){
			move('n', GP);
		}
		else if(followedRoute[j] == 'e'){
			move('e', GP);
		}
		else if(followedRoute[j] == 's'){
			move('s', GP);
		}
		else if(followedRoute[j] == 'w'){
			move('w', GP);
		}

		cout << j << "  " << followedRoute[j] << ":";
		cout << GP.currentLocation.x << "," << GP.currentLocation.y << ";" << GP.direction << "|";
	}

	dockScout(GP);
}

//------------------------------------------MAIN-----------------------------------------------

int main(){
	//Startup stuff.
	signal(SIGINT, exit_signal_handler);
	BP.detect();	//Make sure that the BrickPi3 is communicating and that the filmware is compatible with the drivers/
  BP.reset_all();
  for (int i = 0; i < 5; ++i){
    cout << ".";
    if (i == 3){
      checkSensor();
    }
    sleep(1);
  }
  cout << endl << "Initialized" << endl;

	//Reset the encoders
	BP.offset_motor_encoder(PORT_A, BP.get_motor_encoder(PORT_A));
	BP.offset_motor_encoder(PORT_B, BP.get_motor_encoder(PORT_B));
	BP.offset_motor_encoder(PORT_C, BP.get_motor_encoder(PORT_C));
	BP.offset_motor_encoder(PORT_D, BP.get_motor_encoder(PORT_D));

	//Read the encoders
	int32_t EncoderA = BP.get_motor_encoder(PORT_A);
	int32_t EncoderB = BP.get_motor_encoder(PORT_B);
	int32_t EncoderC = BP.get_motor_encoder(PORT_C);
	int32_t EncoderD = BP.get_motor_encoder(PORT_D);

	//Use the encoder value from motor A to control motors B, C, and D
	BP.set_motor_power(PORT_B, EncoderA < 100 ? EncoderA > -100 ? EncoderA : -100 : 100);
	BP.set_motor_dps(PORT_C, EncoderA);
	BP.set_motor_position(PORT_D, EncoderA);	
	
	int uChoice;
	char message[256];

	gridPoints GP;
	GP.direction = 'n';
	range obstakel;
	vector<vector<bool>> grid = getGrid(GP);
	string followedRoute;
	bool destinationArrived = false;

  while(::running){
    cout << "Kies functie: " << endl;
		cout << "0: Exit" << endl;
    cout << "1: Send message" << endl;
    cout << "2: Set communication details" << endl;
    cout << "3: Check sensor" << endl;
		cout << "4: Auto path." << endl;
		cout << "5: Manual pathing." << endl;

    cin >> uChoice;
		cout << "|==================================================|" << endl;

    switch(uChoice) {
			case 0:
				::running = 0;
				break;
      case 1:
        cout << "Message: " << endl;
        cin >> message;
        iClient(message);
        break;
      case 2:
        SetComm();
        break;
      case 3:
        checkSensor();
        break;
			case 4:
				followRoute(followedRoute, destinationArrived, GP, grid, obstakel);
				cout << "followed route" << endl;
				//communicate(followedRoute);
				driveBack(followedRoute, GP);
				resetCurrentLocation(GP);
			case 5:
				getCoordinates(GP, grid);
				moveToHomepoint(GP);
				resetCurrentLocation(GP);
				followedRoute = manualControl(GP);
				strcpy(message, followedRoute.c_str());
				cout << message << " ";
    }
	}
	//moveForward();
	cout << "exit(0)";
	return 0;
}
