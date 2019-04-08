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

bool battery = true;          //battery level function
int ComPortNr = 6969;         //Port number for communication
char ComHostName[] = "dex2";  //Hostname for communication

void exit_signal_handler(int signo);

/*
  Author:       Gerjan
  Description:  Functie voor het vragen en aanpassen van de hostname en de port voor communicatie met de server.
*/
void SetComm(){
  cout << endl << "Geef het poort-nummer op: ";
  cin >> ::ComPortNr;
  cout << endl << "Geef de host-name op: ";
  cin >> ::ComHostName; cout << endl;
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
  Author:       Duur
  Description:  Verstuur bericht naar opgegeven hostname en port, neemt input van een string en verzend die via STREAM naar server-host.
*/
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

void Drive(vector<char> & route){
  for (int i = 0; i < route.size; ++i) {
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
}





int main(){
  signal(SIGINT, exit_signal_handler); // register the exit function for Ctrl+C
  BP.detect();
  BP.reset_all();
  cout << endl << "Initialized" << endl;

  int uChoice;

  while (true){
    cout << "Kies functie: " << endl;
    cout << "1: Send message" << endl;
    cout << "2: Set communication details" << endl;
    cout << "3: Check sensor" << endl;

    cin >> uChoice;

    switch(uChoice) {
      case 1:
        char message[256];
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