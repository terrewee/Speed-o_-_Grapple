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
#include <vector>

using namespace std;

//------------------------------------------ESSENSIALS-----------------------------------------------

BrickPi3 BP;

// Signal handler that will be called when Ctrl+C is pressed to stop the program
void exit_signal_handler(int signo){
  if(signo == SIGINT){
    BP.reset_all();    // Reset everything so there are no run-away motors
    exit(-2);
  }
}

void setSensor(){
  BP.set_sensor_type(PORT_1,SENSOR_TYPE_NXT_ULTRASONIC);
  BP.set_sensor_type(PORT_2,SENSOR_TYPE_NXT_COLOR_FULL);
  BP.set_sensor_type(PORT_3,SENSOR_TYPE_NXT_LIGHT_ON);
  BP.set_sensor_type(PORT_4,SENSOR_TYPE_NXT_COLOR_FULL);
}

void checkSensor() {
  sensor_color_t        Color;
  sensor_ultrasonic_t   Ultrasonic;
  sensor_light_t        Light;
  sensor_touch_t        Touch;

  int portNr = 0;
  uint8_t portValue = 0;
  int sensorNr = 0;
  int nTimes = 1;

  cout << "Geef poort nummer (1-4): " << endl;
  cin >> portNr;
  cout << "Geef sensor type: " << endl;
  cout << "1: Color" << endl;
  cout << "2: Ultrasonic" << endl;
  cout << "3: Light" << endl;
  cout << "4: Touch" << endl;
  cin >> sensorNr;
  cout << "Hoe vaak checken: " << endl;
  cin >> nTimes;

  if(portNr == 1) portValue = 0x01;
  else if (portNr == 2) portValue = 0x02;
  else if (portNr == 3) portValue = 0x03;
  else if (portNr == 4) portValue = 0x04;
  else { cout << "Geen juiste keuze ontvangen.";}

  switch(sensorNr) {
    case 1:
      for (int i = 0; i < nTimes; ++i) {
        if(BP.get_sensor(portValue,Color) == 0) {
          cout << " red : " << Color.reflected_red;
          cout << " green : " << Color.reflected_green;
          cout << " blue : " << Color.reflected_blue;
          cout << " ambient : " << Color.ambient << endl;
          cout << " color code : " << (int) Color.color << endl;
        }
        sleep(5);
      }
      break;
    case 2:
      cout << "Case 2" << endl;
      for (int i = 0; i < nTimes; ++i) {
        cout << "In de case 2 loop nr : " << i << endl;
        if(BP.get_sensor(portValue,Ultrasonic) == 0) {
          cout << Ultrasonic.cm << " cm" << endl;
        }
        sleep(5);
      }
      break;
    case 3:
      for (int i = 0; i < nTimes; ++i) {
        if(BP.get_sensor(portValue,Light) == 0) {
          cout << " ambient : " << Light.ambient << endl;
          cout << " reflected : " << Light.reflected << endl;
        }
        sleep(5);
      }
      break;
    case 4:
      for (int i = 0; i < nTimes; ++i) {
        if(BP.get_sensor(portValue,Touch) == 0) {
          cout << "pressed : " << Touch.pressed << endl;
        }
        sleep(5);
      }
      break;
  }
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

// bool crossroaddetectie2(){
//   sensor_color_t      Color2;
//   sensor_color_t      Color4;
//   while (::running){
//     if((BP.get_sensor(PORT_2, Color2) == 0) && (BP.get_sensor(PORT_4, Color4) == 0)){
//     	if (Color2.color == 1 || Color4.color == 1){
//         usleep(200000); // sleep van 100 ms zodat hetzelfde kruispunt niet tweemaal wordt geregistreerd
//         return true;
//       }
//     }
//   }
// }

struct routeCount {
  vector<char> direction = {};
  vector<int> amount = {};
};


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
	cout << "getcoord " << GP.homeCoordinates.x << "," << GP.homeCoordinates.y << endl;
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

void resetMotors(){
	BP.set_motor_power(PORT_B, 0);
	BP.set_motor_power(PORT_C, 0);
}

void moveForward(int lspd, int rspd){
	BP.set_motor_power(PORT_B,-lspd);
	BP.set_motor_power(PORT_C,-rspd);
	sleep(2);
}

//Turns the rorbot to the right, and updates the value of GP.direction.
void turnLeft(gridPoints & GP){
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

	BP.get_motor_encoder(PORT_B);
	BP.get_motor_encoder(PORT_C);
	BP.set_motor_position_relative(PORT_B, -60);
	BP.set_motor_position_relative(PORT_C, 110);
	sleep(1);
}

//Turns the rorbot to the left, and updates the value of GP.direction.
void turnRight(gridPoints & GP){
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

	BP.get_motor_encoder(PORT_B);
	BP.get_motor_encoder(PORT_C);
	BP.set_motor_position_relative(PORT_B, 110);
	BP.set_motor_position_relative(PORT_C, -60);
	sleep(1);
}

//-------line intructions---------

bool stopVoorObject(){
	sensor_ultrasonic_t Ultrasonic1;
	if(BP.get_sensor(PORT_1, Ultrasonic1) == 0)
	{
		if(Ultrasonic1.cm <= 20)
		{
			return true;
		}
		return false;
	}
  return false;
}

//Moves robot set amount of crossroads forwards, aantalKeerTeGaan = aantal keer dat de scout 1 kant op moet.

void followLine(int aantalKeerTeGaan) // aantalKeerTeGaan = aantal keer dat de scout 1 kant op moet
{
    sensor_light_t Light3;
    sensor_color_t Color2;
    sensor_color_t Color4;
    sensor_ultrasonic_t Ultrasonic1;

    int offset = 45;
    int Tp = 25;
    int Kp = 4;

    int lastError = 0;
    int Turn = 0;
    int lightvalue = 0;
    int error = 0;

    int lspd = 0;
    int rspd = 0;

    int lastColor2 = 0;
    int lastColor4 = 0;
    int crossroads = 0;

    while(true) {
        if (BP.get_sensor(PORT_2, Color2) == 0 && BP.get_sensor(PORT_4, Color4) == 0) {
            cout << "I am in check" << endl;
            if (Color2.color <= 2 || Color4.color <= 2 ) {
             cout << "Got a crossroads" << endl;
             crossroads++;
             usleep(200000);
            }
        }
        cout << crossroads << " Crossroads" << endl;
        if(BP.get_sensor(PORT_3, Light3) == 0) {
            if(crossroads == aantalKeerTeGaan) {
                crossroads = 0;
                resetMotors();
                break;
            }
            lightvalue = Light3.reflected;
						cout << lightvalue << endl;
            error = ((lightvalue-1700)/40)+30 - offset;
						// cout << "error: " << error << endl;

            Turn = error * Kp;
            Turn = Turn;

            lspd = Tp + Turn;
            rspd = Tp - Turn;

            if (BP.get_sensor(PORT_1,Ultrasonic1) == 0) {
                if(Ultrasonic1.cm < 20){
                    resetMotors();
                    sleep(1);
                    continue;
                }else if(Ultrasonic1.cm < 40){
                    lspd = lspd / 2;
                    rspd = rspd / 2;
                }
            }

            if(crossroads == aantalKeerTeGaan - 1) {
                lspd = lspd / 2;
                rspd = rspd / 2;
            }
            moveForward(lspd,rspd);
            lastError = error;
            cout << "Crossroad " << crossroads << endl;
            cout << "lspd: " << lspd << endl << "rspd: " << rspd << endl;
        }
    }
    resetMotors();
}


//-------path instructions--------

//Sets GP.currentCoordinates to GP.homeCoordinates (homepoint coordinates.)
void resetCurrentLocation(gridPoints &GP){
  GP.currentLocation.x = GP.homeCoordinates.x;
  GP.currentLocation.y = GP.homeCoordinates.y;
	cout << "reset " << GP.homeCoordinates.x << "," << GP.homeCoordinates.y << endl;
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
    GP.currentLocation.x += distance;
  }
}

//Moves robot a set distance forward and calls updateLocation().
void moveForwardDistance(gridPoints &GP, unsigned int distance){
	cout << "Pre-followLine()" << endl;
  followLine(distance);
	cout << "Post-followLine()" << endl;
  updateLocation(GP, distance);
  cout << "moveForwardDistance: "<< distance << endl;

}

//Moves to homepoint with line assistance.
void moveToHomepoint(gridPoints GP){
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

//Moves to homepoint without line assistance.
void moveToHomepointVirtual(gridPoints GP){
	GP.direction = 'n';
	if(GP.targetCoordinates.y == 0 && GP.targetCoordinates.x == 0){/*communicate();*/}
	turnLeft(GP);
 	moveForward(25,25);
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
		if 			(answer == "w")		{followLine(1);}
		else if (answer == "a")		{turnLeft(GP); followLine(1);}
		else if (answer == "d")		{turnRight(GP); followLine(1);}
		else if (answer == "esc")	{break;}
		else 											{cout << "invalid input." << endl; continue;}

		//if(crossroad == 1){resetMotors();}
		orientationList.push_back(GP.direction);
		cout << GP.direction << endl << endl;
	}
	string message(orientationList.begin(), orientationList.end());
	return message;
}

//------------------------------------------GRID-----------------------------------------------

routeCount initRouteCount(const string & myRoute) {
  routeCount tStruct;
  tStruct.direction.push_back(' ');
  tStruct.amount.push_back(0);
  int sIndex = 0;
  for(char direction : myRoute){
    if (tStruct.direction[sIndex] == direction) tStruct.amount[sIndex]++;
    else if (tStruct.direction[sIndex] == ' ') {tStruct.direction[sIndex] = direction; tStruct.amount[sIndex]++;}
    else {tStruct.direction.push_back(direction); tStruct.amount.push_back(1); sIndex++;}
  }
  return tStruct;
}

void move(char direction, gridPoints & GP){

	turn(direction, GP);
	cout << "move() >> turn()" << endl;
	moveForwardDistance(GP, 1);
	cout << "move >> moveForwardDistance()" << endl;
}

//Gets the coordinates of a gridPoint from its number.
coordinates getGridPointCoordinates(unsigned int number, vector<vector<bool>> & grid){
	unsigned int columnAmount = grid.size();
	unsigned int rowAmount = grid[0].size();
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
	if		(pathCheck.x < 0)						{return 0;}
	else if	(pathCheck.x > grid[0].size()-1)		{return 0;}
	else if	(pathCheck.y < 0)						{return 0;}
	else if	(pathCheck.y > grid.size()-1)			{return 0;}
	else if	(grid[pathCheck.y][pathCheck.x] == 0) 	{return 0;}
	else 											{return 1;}
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

//Follows route with line assistance.
void followRoute(string & followedRoute, bool & destinationArrived, gridPoints & GP, vector<vector<bool>> grid, range & obstacles){
	string directions;
	bool obstructed = false;

	searchPath(directions, GP, grid);

	while(!destinationArrived){
		if(obstructed){
			searchPath(directions, GP, grid);
			obstructed = false;
		}

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
				resetMotors();

				if(i == directions.size() - 1){
					destinationArrived = true;
				}
			}

			cout << i << "  " << directions[i] << ":";
			cout << GP.currentLocation.x << "," << GP.currentLocation.y << ";" << GP.direction << "|";
		}

	}
}

//FollowRoute() without line assisted driving.
void followRouteVirtual(string & followedRoute, bool & destinationArrived, gridPoints & GP, vector<vector<bool>> grid, range & obstacles){
	string directions;
	bool obstructed = false;

	searchPath(directions, GP, grid);

	while(!destinationArrived){
		if(obstructed){
			searchPath(directions, GP, grid);
			obstructed = false;
		}

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
				followedRoute += directions[i];
				turn(directions[i], GP);
				cout << "moveForward()" << endl;
				moveForward(25,25);
				resetMotors();

				if(i == directions.size() - 1){
					destinationArrived = true;
				}
			}

			cout << i << "  " << directions[i] << ":";
			cout << GP.currentLocation.x << "," << GP.currentLocation.y << ";" << GP.direction << "|";
		}

	}
}

//Docks scout with line assistance.
void dockScout(gridPoints & GP){
	move('e', GP);
	turn('n', GP);
}

//Docks scout without line assistance.
void dockScoutVirtual(gridPoints & GP){
	turn('e', GP);
	moveForward(25,25);
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

//Driveback without line assistance.
void driveBackVirtual(string followedRoute, gridPoints & GP){
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
			turn('n', GP);
			moveForward(25,25);
		}
		else if(followedRoute[j] == 'e'){
			turn('e', GP);
			moveForward(25,25);
		}
		else if(followedRoute[j] == 's'){
			turn('s', GP);
			moveForward(25,25);
		}
		else if(followedRoute[j] == 'w'){
			turn('w', GP);
			moveForward(25,25);
		}

		cout << j << "  " << followedRoute[j] << ":";
		cout << GP.currentLocation.x << "," << GP.currentLocation.y << ";" << GP.direction << "|";
	}

	dockScoutVirtual(GP);
}




//Prints grid for debugging and testing grid generation.
void testFunctie(gridPoints GP, vector<vector<bool>> grid) {
	for (unsigned int i = 0; i < grid.size(); i++) {
		for (unsigned int j = 0; j < grid[i].size(); j++) {
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

//------------------------------------------MAIN-----------------------------------------------

int main(){
	//Startup stuff.
	signal(SIGINT, exit_signal_handler);
	BP.detect();	//Make sure that the BrickPi3 is communicating and that the filmware is compatible with the drivers/
  BP.reset_all();
  for (int i = 0; i < 5; ++i){
    cout << ".";
    if (i == 3){
      setSensor();
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

	int uChoice;
	char message[256];

	gridPoints GP;
	GP.direction = 'n';
	range obstakel;
	vector<vector<bool>> grid = getGrid(GP);
	string followedRoute;
	bool destinationArrived = false;
  bool runProgram = true;
	getCoordinates(GP, grid);

  while(runProgram){


 		//followLine(2);	// 2 voor testje -- pas dit dus aan met de mee te geven parameter

    cout << "Kies functie: " << endl;
		cout << "0: Exit" << endl;
    cout << "1: Send message" << endl;
    cout << "2: Set communication details" << endl;
    cout << "3: Check sensor" << endl;
		cout << "4: Auto path." << endl;
		cout << "5: Manual pathing." << endl;
		cout << "6: Auto path with message" << endl;
		cout << "7: Auto path virtual grid" << endl;

		cout << "Uw keuze is: ";
    cin >> uChoice;
		cout << "|==================================================|" << endl;

    switch(uChoice) {
			case 0:
        {runProgram = false;}
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
				// testFunctie(GP, grid);
				moveToHomepoint(GP);
				resetCurrentLocation(GP);
				followRoute(followedRoute, destinationArrived, GP, grid, obstakel);
				cout << "followed route" << endl;
				driveBack(followedRoute, GP);
				resetCurrentLocation(GP);
				//iClient(followedRoute);
				break;
			case 5:
				moveToHomepoint(GP);
				resetCurrentLocation(GP);
				followedRoute = manualControl(GP);
				strcpy(message, followedRoute.c_str());
				cout << message << " ";
				//iClient(message);
				break;
			case 6:
				// testFunctie(GP, grid);
				resetCurrentLocation(GP);
				followRoute(followedRoute, destinationArrived, GP, grid, obstakel);
				cout << "followed route" << endl;
				driveBack(followedRoute, GP);
				resetCurrentLocation(GP);
				strcpy(message, followedRoute.c_str());
				iClient(message);
				break;
			case 7:
				// testFunctie(GP, grid);
				moveToHomepointVirtual(GP);
				resetCurrentLocation(GP);
				followRouteVirtual(followedRoute, destinationArrived, GP, grid, obstakel);
				cout << "followed route" << endl;
				driveBackVirtual(followedRoute, GP);
				cout << "driving back" << endl;
				resetCurrentLocation(GP);
				cout << "Location reset" << endl;
				//iClient(followedRoute);
				runProgram = 0;
				break;
    }
	}
	cout << "exit(0)";
  BP.reset_all();
	return 0;
}