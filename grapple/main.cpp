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

using namespace std;

BrickPi3 BP;

int ComPortNr = 6969;         //Port number for communication
char ComHostName[] = "dex2";  //Hostname for communication

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
  n = write(newSocketFD,"We received the message",23);

  if (n < 0) {
    error("ERROR writing to socket");
  }

  close(newSocketFD);
  close(socketFD);
}

void Drive(char & direction){
    if (route[i] = 'n'){

    }
    if (route[i] = 's'){

    }
    if (route[i] = 'o'){

    }
    if (route[i] = 'w'){

    }
    else{
        cout << "Invalid operator: " << route[i] << endl;
    }
}


void Navigation(vector<char> & route){
    for (int i = 0; i < route.size; ++i) {
        Drive(route[i])
    }
    cout << "Arrived at destination" << endl;
}








int main(){
  signal(SIGINT, exit_signal_handler); // register the exit function for Ctrl+C
  BP.detect();
  BP.reset_all();

  cout << endl << "Initialized" << endl;
    sleep(4);
  cout << endl << "Initialized" << endl;

  int uChoice;

  while (true){
    cout << "Kies functie: " << endl;
    cout << "1: Receive message" << endl;
    cout << "2: Set communication details" << endl;
    cout << "3: Check sensor" << endl;

    cin >> uChoice;

    switch(uChoice) {
      case 1:
        iServer();
        break;
      case 2:
        SetComm();
        break;
      case 3:
        checkSensor();
        break;
    }
  }

  BP.reset_all();
  return 0;
}
// Signal handler that will be called when Ctrl+C is pressed to stop the program
void exit_signal_handler(int signo){
  if(signo == SIGINT){
    BP.reset_all();    // Reset everything so there are no run-away motors
    exit(-2);
  }
}