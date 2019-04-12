#include "BrickPi3.h"     // for BrickPi3 header file

#include <string>
#include <vector>
#include <iostream>

#include <stdlib.h>
#include <stdio.h>        // for printf
#include <unistd.h>       // for usleep
#include <signal.h>       // for catching exit signals
#include <sys/socket.h>   // include voor het gebruik van sockets
#include <netinet/in.h>   // include voor het gebruik van sockets
#include <netdb.h>        // include voor het gebruik van sockets
#include <sstream>        // for converting char256 to vector

using namespace std;
bool running = true;

//---------------------------------------ESSENTIALS---------------------------------------------

BrickPi3 BP;                                                      // Initializing BP

void exit_signal_handler(int signo);                              // For initializing the Ctrl + C exit handler

void setSensors() {
    BP.set_sensor_type(PORT_1,SENSOR_TYPE_NXT_LIGHT_ON);
    BP.set_sensor_type(PORT_2,SENSOR_TYPE_NXT_COLOR_FULL);
    BP.set_sensor_type(PORT_3,SENSOR_TYPE_NXT_COLOR_FULL);
}

void resetMotor() {
    BP.set_motor_dps(PORT_A, 0);
    BP.set_motor_dps(PORT_B, 0);
    BP.set_motor_dps(PORT_C, 0);
    BP.set_motor_dps(PORT_D, 0);
    BP.set_motor_position_relative(PORT_A, 0);
    BP.set_motor_position_relative(PORT_B, 0);
    BP.set_motor_position_relative(PORT_C, 0);
    BP.set_motor_position_relative(PORT_D, 0);
    BP.set_motor_power(PORT_A, 0);
    BP.set_motor_power(PORT_B, 0);
    BP.set_motor_power(PORT_C, 0);
    BP.set_motor_power(PORT_D, 0);
    sleep(1);
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

//---------------------------------------ARM---------------------------------------------

void encodeMotorB(int32_t pos) {
    BP.set_motor_position_relative(PORT_B, -pos);
}


void terugVanKantelPunt() {
    BP.set_motor_limits(PORT_B, 40, 0);
    encodeMotorB(-50);
    resetMotor();
    sleep(1);
}


void brengNaarKantelPunt() {
    BP.set_motor_limits(PORT_B, 40, 0);
    encodeMotorB(70);
    sleep(1);
}


void gelijdelijkDownLoop() {
    int32_t encoder = -60;
    while(encoder > -100) {
        encodeMotorB(10);
        usleep(500000);
        encoder = encoder - 10;
    }
    while(encoder > -170) {
        encodeMotorB(5);
        usleep(500000);
        encoder = encoder - 5;
    }
}


void klauwOmhoog() {
    BP.set_motor_limits(PORT_B, 80, 0);
    encodeMotorB(-100);  // zelfde als totale neerwaartse beweging
    sleep(1);
    BP.set_motor_limits(PORT_B, 30, 0);
    encodeMotorB(-50);  // zelfde als totale neerwaartse beweging
    sleep(1);
}


void klauwOpen() {
    BP.set_motor_limits(PORT_D, 60, 0);
    BP.set_motor_position_relative(PORT_D, -180);
    sleep(1);
}


void klauwDicht() {
    BP.set_motor_limits(PORT_D, 60, 0);
    BP.set_motor_position_relative(PORT_D, 180);
    sleep(1);
}


//---------------------------------------COMMUNICATION---------------------------------------------

int ComPortNr = 6969;         //Standaard Port number for communication
char ComHostName[] = "dex2";  //Standaard Hostname for communication

void SetComm() {
    cout << endl << "Geef het poort-nummer op: ";
    cin >> ::ComPortNr; cout << endl;
}

void error(const char *msg) {
    perror(msg);
    exit(1);
}

vector<char> iServer() {

    int socketFD, newSocketFD, n;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;

    socketFD = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(::ComPortNr);

    if (bind(socketFD, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("ERROR on binding");
    }

    cout << "Open for communication, awaiting message" << endl;

    listen(socketFD,5);
    clilen = sizeof(cli_addr);
    newSocketFD = accept(socketFD, (struct sockaddr *) &cli_addr, &clilen);

    if (newSocketFD < 0) {
        error("ERROR on accept");
    }

    bzero(buffer,256);
    n = read(newSocketFD,buffer,255);

    if (n < 0) {
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

    //zet char256 om naar vector met chars
    vector<char> route;
    stringstream ssTemp;
    string target;
    ssTemp << buffer;
    ssTemp >> target;

    for(unsigned int i=0; i < target.size(); i++) {
        route.push_back(target[i]);
    }

    cout << "route received and decoded, time to roll" << endl;
    return route;  //returned een vector<char> met de route (wss naar de Navigation() functie)
}


//---------------------------------------COLOR_RECOGNITION---------------------------------------------

int whatIsInAColor () {
    int color;
    int colorchoice = -1;
    cout << "Welke kleur heeft het object" << endl;
    cout << "1 : Rood " << endl;
    cout << "2 : Blauw " << endl;
    cout << "3 : Groen " << endl;
    cout << "4 : Zwart " << endl;
    cout << "5 : Wit " << endl;
    cin >> color;
    cout << color;
    if (color == 1)          { colorchoice = 5;}
    else if (color == 2)     { colorchoice = 2;}
    else if (color == 3)     { colorchoice = 3;}
    else if (color == 4)     { colorchoice = 1;}
    else if (color == 5)     { colorchoice = 6;}
    return colorchoice;
}


bool colorNotCorrect() {
    string answer;
    cout << "Dit object heeft niet de door u opgegeven kleur. Wilt u het object alsnog oppakken?" << endl;
    cout << "Ja of Nee: ";
    cin >> answer;
    if (answer == "Ja" ){
        cout << "true" << endl;
        return true;
    }
    else if (answer == "nee") {
        cout << "false" << endl;
        return false;
    }
}

bool color_object (int colorchoice){
    sensor_color_t Color2;
    while(colorchoice == -1){
        cout << "Er is geen kleur gegeven." << endl;
        colorchoice = whatIsInAColor();
    }
    if (BP.get_sensor(PORT_2, Color2) == 0) {
        cout << Color2.color << " sensor ," << colorchoice << " color choice"<< endl;
        if (Color2.color == colorchoice) {
            cout << "true" << endl;
            return true;
        }
        else if (Color2.color != colorchoice) {
            return colorNotCorrect();
        }
    }
}


//---------------------------------------DRIVING---------------------------------------------


void fwd(const int lspd, const int rspd) {
    BP.set_motor_power(PORT_A, lspd);
    BP.set_motor_power(PORT_C, rspd);
}


void backUpFromObject(){
    fwd(-15, -15);
    sleep(2);
    resetMotor();
}


void turnLeft() {
    sensor_light_t Light1;
    fwd(-20, -20);
    sleep(2);
    resetMotor();
    sleep(0.5);
    fwd(30, 8);
    sleep(4);
    resetMotor();
}


void turnRight() {
    sensor_light_t Light1;
    fwd(-20, -20);
    sleep(2);
    resetMotor();
    sleep(0.5);
    fwd(8, 30);
    sleep(4);
    resetMotor();
}


int moveForward() {
    //Aan de hand van pid controller
    sensor_light_t Light1;
    sensor_color_t Color3;

    fwd(20, 20); // zorg dat de sensor over de lijn komt zodat hij deze niet voor een ander kruispunt aanziet.
    sleep(1);

    int offset = 45;
    int Tp = 25;

    int Kp = 4;

    int Turn = 0;
    int lightvalue = 0;
    int error = 0;

    int lspd = 0;
    int rspd = 0;

    while (running) {
        if (BP.get_sensor(PORT_1, Light1) == 0) {
            lightvalue = Light1.reflected; // neem waarde van zwartwit sensor
            if (BP.get_sensor(PORT_3, Color3) == 0) {
                if ((Color3.reflected_red < 300) && (lightvalue > 2700)) { // als de zwartwit sensor en de kleur sensor zwart zijn is er een kruispunt
                    cout << "hier is een kruispunt" << endl;
                    return 0;
                }
            }
        }

        error = ((lightvalue - 2100) / 40) + 30 - offset;

        Turn = error * Kp;

        lspd = Tp + Turn;
        rspd = Tp - Turn;
        fwd(lspd, rspd);
    }
}



void drive(char direction) {
    if (direction == 'f') {
        //ga 1 grid plek
        moveForward();
    }
    else if (direction == 'r') {
        //ga 90 graden links
        turnRight();
    }
    else if (direction == 'l') {
        //ga 90 graden links
        turnLeft();
    }
    else if (direction == 'b') {
        //ga 180 graden draaien
        turnRight();
        turnRight();
    } else {
        cout << "Invalid operator: " << direction << endl << "Next time use: f, r, l or b" << endl;
    }
}


void navigation(vector<char> route) {
    bool gotAObject = true;
    route.insert(route.begin(), 1, 'n');    //zorg dat ook het eerste echte coordinaat een relatief punt heeft om vanaf te bewegen
    for (int i = 1; i < route.size(); ++i) { // rij naar het object toe aan de hand van de route
        if (route[i] == route[i-1]) {
            drive('f');
        } else {
            resetMotor();
            if
                        ((route[i] == 'n' && route[i-1] == 'w') ||
                         (route[i] == 'e' && route[i-1] == 'n') ||
                         (route[i] == 's' && route[i-1] == 'e') ||
                         (route[i] == 'w' && route[i-1] == 's')) {
                    drive('r');
                    drive('f');
                }
                else if
                        ((route[i] == 'n' && route[i-1] == 'e') ||
                         (route[i] == 'e' && route[i-1] == 's') ||
                         (route[i] == 's' && route[i-1] == 'w') ||
                         (route[i] == 'w' && route[i-1] == 'n')) {
                    drive('l');
                    drive('f');
                }
                else if
                        ((route[i] == 'n' && route[i-1] == 's') ||
                         (route[i] == 'e' && route[i-1] == 'w') ||
                         (route[i] == 's' && route[i-1] == 'n') ||
                         (route[i] == 'w' && route[i-1] == 'e')) {
                    drive('b');
                    drive('f');
            } else { // als geen van de opties gepakt word gebeurt er iets abnormaals
                cout << "help, er gebeurt iets geks: " << route[i] << route[i-1] << endl;
            }
        }
    }

    resetMotor();
    cout << "Arrived at destination" << endl;
    sleep(1);
    
    //***************************************************************************************************************
                    // pick up object
    if (color_object(whatIsInAColor())) {
        cout << "Pak het op" << endl;
        backUpFromObject();
        resetMotor();
        sleep(1);
        brengNaarKantelPunt();
        klauwOpen();
        gelijdelijkDownLoop();
        klauwDicht();
        sleep(0.2);
        klauwOmhoog();
        resetMotor();
        brengNaarKantelPunt();
        resetMotor();
        drive('b');
        resetMotor();
        cout << "Picked up ze object, time to head back" << endl;
    }
    else {
      cout << "Pak het niet op ga terug" << endl;
    }

    //***************************************************************************************************************

    route.push_back('n'); //zorg dat ook het eerste echte coordinaat een relatief punt heeft om vanaf te bewegen
    for (int i = (route.size() - 2); i > 0; --i) { // rij terug naar het startpunt aan de hand van de route
        if (route[i] == route[i+1]) {
            drive('f');
        } else {
            resetMotor();
            sleep(1);
            if
                    ((route[i] == 'n' && route[i+1] == 'w') ||
                     (route[i] == 'e' && route[i+1] == 'n') ||
                     (route[i] == 's' && route[i+1] == 'e') ||
                     (route[i] == 'w' && route[i+1] == 's')) {
                drive('r');
                drive('f');
            }
            else if
                    ((route[i] == 'n' && route[i+1] == 'e') ||
                     (route[i] == 'e' && route[i+1] == 's') ||
                     (route[i] == 's' && route[i+1] == 'w') ||
                     (route[i] == 'w' && route[i+1] == 'n')) {
                drive('l');
                drive('f');
            }
            else if
                    ((route[i] == 'n' && route[i+1] == 's') ||
                     (route[i] == 'e' && route[i+1] == 'w') ||
                     (route[i] == 's' && route[i+1] == 'n') ||
                     (route[i] == 'w' && route[i+1] == 'e')) {
                drive('b');
                drive('f');
            } else {  // als geen van de opties gepakt word gebeurt er iets abnormaals
                cout << "help, er gebeurt iets geks: " << route[i] << route[i+1] << endl;
            }
        }
    }

    resetMotor();
}


//---------------------------------------MAIN---------------------------------------------

int main() {
    signal(SIGINT, exit_signal_handler); // register the exit function for Ctrl+C
    BP.detect();
    BP.reset_all();

    cout << "Initializing" << endl;

    for (int i = 0; i < 5; ++i) {
      cout << ".";
      if (i == 1) {
        setSensors();
      }
      sleep(1);
    }
    cout << endl << "Initialized" << endl;

    bool runProgram = true;

    BP.set_motor_limits(PORT_A, 50, 0);
    BP.set_motor_limits(PORT_C, 50, 0);

    resetMotor();

    int uChoice;

    while (runProgram) {
        cout << "Kies een van deze functies: " << endl;
        cout << "0: Exit" << endl;                                              // functie om programma te stoppen
        cout << "1: Receive message" << endl;                                   // wacht voor een message en print deze
        cout << "2: Set communication details" << endl;                         // stel portnummer in
        cout << "3: Wait for message (route) , then return the object" << endl; // Er wordt eerst gewacht op een bericht met de coordinaten, die worden daarna gebruikt om het opject op te pakken en terug te rijden
        cout << "4: Check sensor" << endl;
        cout << "5: Drive given road to object" << endl << endl;
        cout << "Uw keuze is: ";

        cin >> uChoice;
        cout << endl << "|==================================================|" << endl << endl;

        switch(uChoice) {
            case 1:{ // wacht voor een message en print deze
                vector<char> testV = iServer();
                for (char character : testV){
                  cout << character << ", ";
                }
                cout << endl << "done" << endl;
                break;}
            case 2: // stel portnummer in
                SetComm();
                break;
            case 3: // Er wordt eerst gewacht op een bericht met de coordinaten,
                    //die worden daarna gebruikt om het opject op te pakken en terug te rijden
                navigation(iServer());
                break;
            case 4: // Check the sensors
                checkSensor();
                break;
            case 5: //test pls dont delete this time duur :D
            {   vector<char> vec = {'n', 'n', 'e', 'e', 'n', 'n' ,'w', 's'};
                navigation(vec);
                break;
            }
            case 6: {
                // pick up object
                if (color_object(whatIsInAColor())) {
                    cout << "Pak het op" << endl;
                    backUpFromObject();
                    resetMotor();
                    sleep(1);
                    brengNaarKantelPunt();
                    klauwOpen();
                    gelijdelijkDownLoop();
                    klauwDicht();
                    sleep(0.2);
                    klauwOmhoog();
                    resetMotor();
                    brengNaarKantelPunt();
                    resetMotor();
                    cout << "Picked up ze object, time to head back" << endl;
                } else {
                    cout << "Pak het niet op ga terug" << endl;
                }
            }
            case 0:{ // functie om programma te stoppen
                runProgram = false;
                break;
              }
        }
    }
}

// Signal handler that will be called when Ctrl+C is pressed to stop the program
void exit_signal_handler(int signo) {
    if(signo == SIGINT){
        resetMotor();
        BP.reset_all(); // Reset everything so there are no run-away motors
        exit(-2);
    }
}
