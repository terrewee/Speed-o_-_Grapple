#include "BrickPi3.h" // for BrickPi3
#include <stdio.h>      // for printf
#include <unistd.h>     // for usleep
#include <signal.h>     // for catching exit signals
#include <stdlib.h>
#include <string.h>     //strings
#include <sys/types.h>  //voor gebruik syscall
#include <sys/socket.h> //voor gebruik sockets
#include <netinet/in.h>
#include <iostream>
#include <thread>

using namespace std;

BrickPi3 BP;

int ComPortNr = 6969;         //Port number for communication
bool battery = true;          //battery level function

void exit_signal_handler(int signo);


/*
  Author:       Duur
  Description:  setSensors set all the sensors for a specific robot
                and immediatly sets them.
*/
void setSensors(){
  //BP.set_sensor_type();
}


/*
  Author:       Maaike & Duur
  Description:  Asks the user to supply a port and a sensor type to check the output
                of said function for a certain amount of time.
*/
void checkSensor(){
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
  else {
    cout << "Geen juiste keuze ontvangen.";
    return;
  }

  switch(sensorNr){
    case 1:
      for (int i = 0; i < nTimes; ++i) {
        if(BP.get_sensor(portValue,Color) == 0){
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
        if(BP.get_sensor(portValue,Ultrasonic) == 0){
          cout << Ultrasonic.cm << " cm" << endl;
        }
        sleep(5);
      }
      break;
    case 3:
      for (int i = 0; i < nTimes; ++i) {
        if(BP.get_sensor(portValue,Light) == 0){
          cout << " ambient : " << Light.ambient << endl;
          cout << " reflected : " << Light.reflected << endl;
        }
        sleep(5);
      }
      break;
    case 4:
      for (int i = 0; i < nTimes; ++i) {
        if(BP.get_sensor(portValue,Touch) == 0){
          cout << "pressed : " << Touch.pressed << endl;
        }
        sleep(5);
      }
      break;
  }

}

/*
  Author:       Maaike & Duur
  Description:  Bateryscheck which changes the
                global bool battery to false if battery is low
*/
void batteryLevel(void){
  //printf("Battery voltage : %.3f\n", BP.get_voltage_battery());
  while(true){
    if(BP.get_voltage_battery() <= 9.0){
      cout << " de batterij is dood. T_T" << endl;
      ::battery = false;
    }
    else{
      ::battery = true;
    }
    sleep(5);
  }
}

void error(const char *msg)
{
  perror(msg);
  exit(1);
}
/*
  Author:       Gerjan
  Description:  Functie voor het vragen en aanpassen van de hostname en de port voor communicatie met de server.
*/
void SetComm(){
  cout << endl << "Geef het poort-nummer op: ";
  cin >> ::ComPortNr; cout << endl;
}
/*
  Author:       Gerjan & Duur
  Description:  Opens a socket and listens for a message, return a message based on result.
*/
void iServer(){
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
  n = write(newSocketFD,"1",1);

  if (n < 0) {
    error("ERROR writing to socket");
  }

  close(newSocketFD);
  close(socketFD);
}

// Signal handler that will be called when Ctrl+C is pressed to stop the program
void exit_signal_handler(int signo){
  if(signo == SIGINT){
    BP.reset_all();    // Reset everything so there are no run-away motors
    exit(-2);
  }
}

int main()
{
  signal(SIGINT, exit_signal_handler); // register the exit function for Ctrl+C
  BP.detect(); // Make sure that the BrickPi3 is communicating and that the firmware is compatible with the drivers.
  BP.set_motor_limits(PORT_B, 60, 0);
  BP.set_motor_limits(PORT_C, 60, 0);

  setSensors();
  thread checkBattery (batteryLevel);
  int uChoice;

  while (true){
    cout << "Kies functie: " << endl;
    cout << "1: Recieve message" << endl;
    cout << "2: Set communication details" << endl;

    cin >> uChoice;

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
        break;
      case 5:
        break;
      case 6:
        break;
    }
  }
  return 0;
}