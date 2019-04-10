#include "BrickPi3.h"     // for BrickPi3
#include <stdio.h>        // for printf
#include <unistd.h>       // for usleep
#include <signal.h>       // for catching exit signals
#include <stdlib.h>
#include <sys/socket.h>   // include voor het gebruik van sockets
#include <netinet/in.h>   // include voor het gebruik van sockets
#include <netdb.h>        // include voor het gebruik van sockets
#include <string>
#include <iostream>
#include <thread>         // include voor het gebruik van threads
#include <vector>
#include <sstream>        // for converting char256 to vector

using namespace std;



    // initializing some stuff

BrickPi3 BP;

int ComPortNr = 6969;         //Standaard Port number for communication
char ComHostName[] = "dex2";  //StandaardHostname for communication
bool running = true;          //Bool for turning off the program including threads

void exit_signal_handler(int signo); // for initializing the Ctrl + C exit handler


/*
	Author 		:	Duur Alblas
	Description	:   Initialize all sensors
*/

void setSensors(){
    BP.set_sensor_type(PORT_1,SENSOR_TYPE_NXT_COLOR_FULL);
    BP.set_sensor_type(PORT_2,SENSOR_TYPE_NXT_ULTRASONIC);
    BP.set_sensor_type(PORT_3,SENSOR_TYPE_NXT_LIGHT_ON);
    BP.set_sensor_type(PORT_4,SENSOR_TYPE_NXT_COLOR_FULL);
}



//      Functions for communications

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
    return route;  //returned een vector<char> met de route (wss naar de Navigation() functie)
}



//      Functions for driving

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
  Author:       Gerjan
  Description:  Functions for controlling the motors
*/

void resetMotor(){
    BP.set_motor_dps(PORT_B, 0);
    BP.set_motor_dps(PORT_C, 0);
    BP.set_motor_position_relative(PORT_B, 0);
    BP.set_motor_position_relative(PORT_C, 0);
    BP.set_motor_power(PORT_B, 0);
    BP.set_motor_power(PORT_C, 0);
    sleep(1);
}

void fwd(const int lspd, const int rspd){
    BP.set_motor_power(PORT_B, lspd);
    BP.set_motor_power(PORT_C, rspd);
}

void turnLeft(){
    fwd(-20, -20);
    sleep(1);
    resetMotor();
    sleep(0.5);
    fwd(20, -60);
    sleep(7);
}
void turnRight(){
    fwd(-20, -20);
    sleep(1);
    resetMotor();
    sleep(0.5);
    fwd(-60, 20);
    sleep(7);
}

/*
  Author:       Gerjan
  Description:  Pid function which shuts down when it meets an intersection
*/

int moveForward() {
    //Aan de hand van pid controller
    sensor_light_t Light3;
    sensor_color_t Color1;

    fwd(20, 20); // zorg dat de sensor over de lijn komt zodat hij deze niet voor een ander kruispunt aanziet.
    sleep(0.5);

    int offset = 45;
    int Tp = 20;

    int Kp = 4;

    int Turn = 0;
    int lightvalue = 0;
    int error = 0;

    int lspd = 0;
    int rspd = 0;

    while (true) {
        if (BP.get_sensor(PORT_3, Light3) == 0) {
            lightvalue = Light3.reflected; // neem waarde van zwartwit sensor

            if (BP.get_sensor(PORT_1, Color1) == 0){

                if ((Color1.color == 1 || Color1.color == 2) && (lightvalue > 2300)) { // als de zwartwit sensor en de kleur sensor zwart zijn is er een kruispunt
                    cout << "hier is een kruispunt" << endl;
                    return 0;
                }
            }
        }

        error = ((lightvalue - 500) / 110) + 30 - offset;

        Turn = error * Kp;

        // als de Turnsnelheid meer dan 2 keer zo groot word dan de normale rijsnelheid,
        // gaat de robot alleen foccussen op draaien, zonder nog te rijden
        if (Turn > Tp){
            fwd(Tp, -1 * Tp);
        }
        else if (Turn < -1 * Tp){
            fwd(-1 * Tp, Tp);
        } else{
            lspd = Tp + Turn;
            rspd = Tp - Turn;
            fwd(lspd, rspd);
        }
    }
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
    route.insert(route.begin(), 1, 'n');    //zorg dat ook het eerste echte coordinaat een relatief punt heeft om vanaf te bewegen
    for (int i = 1; i < route.size(); ++i) { // rij naar het object toe aan de hand van de route
        if (route[i] == route[i-1]){
            Drive('f');
        }
        else {
            resetMotor();
            if
                ((route[i] == 'n' && route[i-1] == 'w') ||
                (route[i] == 'o' && route[i-1] == 'n') ||
                (route[i] == 's' && route[i-1] == 'o') ||
                (route[i] == 'w' && route[i-1] == 's')){
                Drive('r');
                Drive('f');
            }
            else if
                ((route[i] == 'n' && route[i-1] == 'o') ||
                (route[i] == 'o' && route[i-1] == 's') ||
                (route[i] == 's' && route[i-1] == 'w') ||
                (route[i] == 'w' && route[i-1] == 'n')){
                Drive('l');
                Drive('f');
            }
            else if
                ((route[i] == 'n' && route[i-1] == 's') ||
                (route[i] == 'o' && route[i-1] == 'w') ||
                (route[i] == 's' && route[i-1] == 'n') ||
                (route[i] == 'w' && route[i-1] == 'o')){
                Drive('b');
                Drive('f');
            } else{ // als geen van de opties gepakt word gebeurt er iets abnormaals
                cout << "help, er gebeurt iets geks: " << route[i] << route[i-1] << endl;
            }
        }
    }


    cout << "Arrived at destination" << endl;
    //***************************************************************************************************************
     sleep(10);                                             //functie voor object zien en vooral oppakken
    //***************************************************************************************************************
    cout << "Picked up ze object, time to head back" << endl;


    route.push_back('n'); //zorg dat ook het eerste echte coordinaat een relatief punt heeft om vanaf te bewegen
    for (int i = (route.size() - 2); i > 0; --i) { // rij terug naar het startpunt aan de hand van de route
        if (route[i] == route[i+1]){
            Drive('f');
        }
        else {
            resetMotor();
            sleep(1);
            if
                ((route[i] == 'n' && route[i+1] == 'w') ||
                (route[i] == 'o' && route[i+1] == 'n') ||
                (route[i] == 's' && route[i+1] == 'o') ||
                (route[i] == 'w' && route[i+1] == 's')){
                Drive('l');
                Drive('f');
            }
            else if
                ((route[i] == 'n' && route[i+1] == 'o') ||
                (route[i] == 'o' && route[i+1] == 's') ||
                (route[i] == 's' && route[i+1] == 'w') ||
                (route[i] == 'w' && route[i+1] == 'n')){
                Drive('r');
                Drive('f');
            }
            else if
                ((route[i] == 'n' && route[i+1] == 's') ||
                (route[i] == 'o' && route[i+1] == 'w') ||
                (route[i] == 's' && route[i+1] == 'n') ||
                (route[i] == 'w' && route[i+1] == 'o') ){
                Drive('b');
                Drive('f');
            } else{  // als geen van de opties gepakt word gebeurt er iets abnormaals
                cout << "help, er gebeurt iets geks: " << route[i] << route[i+1] << endl;
            }
        }
    }
    resetMotor();
    //***************************************************************************************************************
    sleep(10);                                             //fucntie voor object droppen
    //***************************************************************************************************************
    cout << "Arrived home, dropping the object like its hot" << endl;
    Drive('b'); // orienteer jezelf goed voor de volgende missie
}


//  main to initialize the program

int main(){
  signal(SIGINT, exit_signal_handler); // register the exit function for Ctrl+C
  BP.detect();
  BP.reset_all();

  BP.set_motor_limits(PORT_B, 50, 0);
  BP.set_motor_limits(PORT_C, 50, 0);
  setSensors();

  // mooie manier om even te wachten tot alle sensor zijn ge initialiseerd en goed werken.
  cout << endl << "Initializing" << endl;
  int o = 10;
  while (o != 0){
      --o;
      cout << '.';
      sleep(0.5);
  }
  cout << endl << "Initialized" << endl << endl;


  int uChoice;

  while (running){
    cout << endl << "Kies een van deze functies: " << endl;
    cout << "1: Receive message" << endl; // wacht voor een message en print deze
    cout << "2: Set communication details" << endl;  // stel portnummer in
    cout << "3: Wait for message (route) , then return the object" << endl; // Er wordt eerst gewacht op een bericht met de coordinaten, die worden daarna gebruikt om het opject op te pakken en terug te rijden
    cout << "4: Drive the given route" << endl;  // dit is een case die gebruikt wordt voor testing en demo's maar kan weg weg in eindproduct
    cout << "0: Stop it and die" << endl << endl; // functie om programma te stoppen
    cout << "Uw keuze is: ";

      cin >> uChoice; cout << endl << endl;

    switch(uChoice) {
      case 1:   // wacht voor een message en print deze
        iServer();
        break;
      case 2:   // stel portnummer in
        SetComm();
        break;
      case 3:    // Er wordt eerst gewacht op een bericht met de coordinaten, die worden daarna gebruikt om het opject op te pakken en terug te rijden
        Navigation(iServer());
        break;
      case 4: { // dit is een case die gebruikt wordt voor testing en demo's maar kan weg weg in eindproduct
          vector<char> vec = {'n', 'n', 'w', 'w', 's'};
          Navigation(vec);
          break;
      }
      case 0: {  // functie om programma te stoppen
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
      ::running = false;
      BP.reset_all();    // Reset everything so there are no run-away motors
    exit(-2);
  }
}