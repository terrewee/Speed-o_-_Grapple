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
#include <sstream> //for converting char256 to vector

using namespace std;

BrickPi3 BP;

int ComPortNr = 6969;         //Port number for communication
char ComHostName[] = "dex2";  //Hostname for communication
bool running = true;

void exit_signal_handler(int signo);


/*
  Author:       Gerjan
  Description:  Functie voor het vragen en aanpassen van de hostname en de port voor communicatie met de server.
*/
void SetComm(){
  cout << endl << "Geef het poort-nummer op: ";
  cin >> ::ComPortNr; cout << endl;
}
/*
  Author:       Duur & Gerjan
  Description:  Small function to throw error message
*/
void error(const char *msg) {
  perror(msg);
  exit(1);
}
/*
  Author:       Gerjan & Duur
  Description:  Opens a socket and listens for a message, return a message based on result.
*/
vector<char> iServer(){

  int socketFD, newSocketFD, n;
  socklen_t clilen;
  char buffer[256];
  struct sockaddr_in serv_addr, cli_addr;

  socketFD = socket(AF_INET, SOCK_STREAM, 0);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(::ComPortNr);

  if (bind(socketFD, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
    error("ERROR on binding");
  }

    cout << "Open for communication, awaiting message" << endl;

    listen(socketFD,5);
  clilen = sizeof(cli_addr);
  newSocketFD = accept(socketFD, (struct sockaddr *) &cli_addr, &clilen);

  if (newSocketFD < 0){
    error("ERROR on accept");
  }

  bzero(buffer,256);
  n = read(newSocketFD,buffer,255);

  if (n < 0){
    error("ERROR reading from socket");
  }
  // Received message in the buffer.
  printf("Here is the message: %s\n",buffer);
  // Try to send "1" back so client knows communication succeeded.
  n = write(newSocketFD,"We received the message",23);

  if (n < 0) {
    error("ERROR writing to socket");
  }

  close(newSocketFD);
  close(socketFD);

  //zet char256 om naar vector met chars, gedaan door Stef
  vector<char> route;
  stringstream ssTemp;
  string target;
  ssTemp << buffer;
  ssTemp >> target;

  for(unsigned int i=0; i < target.size(); i++){
    route.push_back(target[i]);
  }

  cout << "route received and decoded, time to roll" << endl;
    return route;
}

/*
	Author		:	Duur Alblas
	Description :
		Short code to set motor encoders.
*/
void encodeMotors(int32_t lpos , int32_t rpos){
    BP.set_motor_position_relative(PORT_B, lpos);
    BP.set_motor_position_relative(PORT_C, rpos);
}

/*
  Author:       Stefan & Gerjan
  Description:  Functions for driving the motors
*/

//Moves robot one grid unit forward, do NOT use this function to move the robot. moveForwardDis1tance() is made for that.

void resetMotor(){
    BP.set_motor_dps(PORT_B, 0);
    BP.set_motor_dps(PORT_C, 0);
    BP.set_motor_position_relative(PORT_B, 0);
    BP.set_motor_position_relative(PORT_C, 0);
    BP.set_motor_power(PORT_B, 0);
    BP.set_motor_power(PORT_C, 0);
    sleep(5);
}

void turnMotorPowerUp(int motorPower) {
    while (motorPower < 20) {
        BP.set_motor_power(PORT_B, motorPower);
        BP.set_motor_power(PORT_C, motorPower);
        motorPower += 3;
        sleep(0.2);
    }
    BP.set_motor_power(PORT_B, 20);
    BP.set_motor_power(PORT_C, 20);
}

void turnMotorPowerDown(int motorPower) {
    while (motorPower > 0) {
        BP.set_motor_power(PORT_B, motorPower);
        BP.set_motor_power(PORT_C, motorPower);
        motorPower -= 4;
        sleep(0.1);
    }
    resetMotor();
}

void moveForward(){
    int motorPower = 0;
    turnMotorPowerUp(motorPower);
    sleep(3); //dit moet afgesteld worden met de speed-o
    turnMotorPowerDown(motorPower);
}

void turnLeft(){
    BP.set_motor_power(PORT_B, -50);
    BP.set_motor_power(PORT_C, 50);
    sleep(5.5);
    resetMotor();
}
void turnRight(){
    BP.set_motor_power(PORT_B, 50);
    BP.set_motor_power(PORT_C, -50);
    sleep(5.5);
    resetMotor();
}

/*
  Author:       Gerjan
  Description:  Takes in an command for the motors and executes it.
*/

void Drive(char direction){
    if (direction == 'f'){
        //ga 1 unit vooruit
        cout << "Forward" << endl;
        moveForward();
    }
    else if (direction == 'r'){
        //ga 90 graden links
        cout << "Right" << endl;
        turnRight();
    }
    else if (direction == 'l'){
        //ga 90 graden links
        cout << "Left" << endl;
        turnLeft();
    }
    else if (direction == 'b'){
        //ga 180 graden draaien
        cout << "Reverse" << endl;
        turnRight();
        turnRight();

    }
    else{
        cout << "Invalid operator: " << direction << endl << "Next time use: f, r, l or b" << endl;
    }
}

/*
  Author:       Gerjan
  Description:  Takes a Vector with chars n,e,s and w,
  and uses those to drive to the destination, pick up an object and drive back.
*/

void Navigation(vector<char> route){
    route.insert(route.begin(), 1, 'n');
    for (int i = 1; i < route.size(); ++i) { // rij naar het object toe aan de hand van de route
        if (route[i] == route[i-1]){
            Drive('f');
        }
        else {
            if ((route[i] == 'n' && route[i-1] == 'w') ||
                (route[i] == 'o' && route[i-1] == 'n') ||
                (route[i] == 's' && route[i-1] == 'o') ||
                (route[i] == 'w' && route[i-1] == 's')){
                Drive('r');
                Drive('f');
            }
            else if ((route[i] == 'n' && route[i-1] == 'o') ||
                (route[i] == 'o' && route[i-1] == 's') ||
                (route[i] == 's' && route[i-1] == 'w') ||
                (route[i] == 'w' && route[i-1] == 'n')){
                Drive('l');
                Drive('f');
            }
            else if ((route[i] == 'n' && route[i-1] == 's') ||
                (route[i] == 'o' && route[i-1] == 'w') ||
                (route[i] == 's' && route[i-1] == 'n') ||
                (route[i] == 'w' && route[i-1] == 'o')){
                Drive('b');
                Drive('f');
            } else{
                cout << "help, er gebeurt iets geks: " << route[i] << route[i-1] << endl;
            }
        }
    }


    cout << "Arrived at destination" << endl;
     sleep(10);                                                                               //functie voor object zien en pakken
    cout << "Picked up ze object, time to head back" << endl;


    Drive('b');
    route.push_back('n');
    sleep(4);
    for (int i = (route.size() - 2); i > 0; --i) { // rij terug naar het startpunt aan de hand van de route
        if (route[i] == route[i+1]){
            Drive('f');
        }
        else {
            if ((route[i] == 'n' && route[i+1] == 'w') ||
                (route[i] == 'o' && route[i+1] == 'n') ||
                (route[i] == 's' && route[i+1] == 'o') ||
                (route[i] == 'w' && route[i+1] == 's')){
                Drive('r');
                Drive('f');
            }
            else if ((route[i] == 'n' && route[i+1] == 'o') ||
                (route[i] == 'o' && route[i+1] == 's') ||
                (route[i] == 's' && route[i+1] == 'w') ||
                (route[i] == 'w' && route[i+1] == 'n')){
                Drive('l');
                Drive('f');
            }
            else if ((route[i] == 'n' && route[i+1] == 's') ||
                (route[i] == 'o' && route[i+1] == 'w') ||
                (route[i] == 's' && route[i+1] == 'n') ||
                (route[i] == 'w' && route[i+1] == 'o') ){
                Drive('b');
                Drive('f');
            } else{
                cout << "help, er gebeurt iets geks: " << route[i] << route[i+1] << endl;
            }
        }
    }
    resetMotor();
    sleep(10);
    cout << "Arrived home, dropping the object like its hot" << endl;
                                                                                             //Drop object
    Drive('b');
}




int main(){
  signal(SIGINT, exit_signal_handler); // register the exit function for Ctrl+C
  BP.detect();
  BP.reset_all();

    BP.set_motor_limits(PORT_B, 50, 0);
    BP.set_motor_limits(PORT_C, 50, 0);

  cout << endl << "Initializing" << endl;
  int o = 10;
  while (o != 0){
      --o;
      cout << '.';
      sleep(0.5);
  }
  cout << endl << "Initialized" << endl;

  int uChoice;

  while (running){
    cout << "Kies een van deze functies: " << endl;
    cout << "1: Receive message" << endl;
    cout << "2: Set communication details" << endl;
    cout << "3: Check sensor" << endl;
    cout << "4: Wait for message (route) , then return the object" << endl;
    cout << "5: Drive the giving route" << endl;
    cout << "6: Stop it and die" << endl << endl;
    cout << "Uw keuze is: ";

      cin >> uChoice; cout << endl << endl;

    switch(uChoice) {
      case 1:
        iServer();
        break;
      case 2:
        SetComm();
        break;
      case 3:
        break;
      case 4:
        Navigation(iServer());
        break;
      case 5: {
          vector<char> vec = {'n', 'w', 'w', 'o', 's', 's'};
          Navigation(vec);
          break;
      }
      case 6: {
          ::running = false;
          break;
      }
    }
  }

  BP.reset_all();
  return 0;
}
// Signal handler that will be called when Ctrl+C is pressed to stop the program
void exit_signal_handler(int signo){
  if(signo == SIGINT){
    BP.reset_all();    // Reset everything so there are no run-away motors
    resetMotor();
    exit(-2);
  }
}