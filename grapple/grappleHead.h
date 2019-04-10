#ifndef GRAPPLEMAIN_H
#define GRAPPLEMAIN_H

#include "BrickPi3.h"
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
#include <iomanip>	// for setw and setprecision

extern BrickPi3 BP;


/*
  Author:       Duur
  Description:  setSensors set all the sensors for a specific robot
                and immediatly sets them.
*/
void setSensors();
/*
  Author:       Maaike & Duur
  Description:  Asks the user to supply a port and a sensor type to check the output
                of said function for a certain amount of time.
*/
void checkSensor();
/*
  Author:       Maaike & Duur
  Description:  Bateryscheck which changes the
                global bool battery to false if battery is low
*/
void batteryLevel(void);
/*
  Author:       Gerjan
  Description:  Functie voor het vragen en aanpassen van de hostname en de port voor communicatie met de server.
*/
void SetComm();
/*
  Author:       Duur & Gerjan
  Description:  Small function to throw error message
*/
void error(const char *msg);
/*
  Author:       Gerjan & Duur
  Description:  Opens a socket and listens for a message, return a message based on result.
*/
void iServer();
/*
	Author 		:	Duur Alblas
	Description	:
		Initialize all sensors
*/
void setSensors();
/*
	Author:		:	Joram van Leeuwen, Duur Alblas
	Description	:
		Short code to set a single motor encoder.
*/
void encodeMotor(int32_t pos);
/*
	Author		:	Joram van Leeuwen
	Description	:
		Functie om arm over kantelpunt te krijgen
*/
void brengNaarKantelPunt();
/*
	Author		:	Joram van Leeuwen, Stef Ottenhof
	Description	:
		Functie om klauw gelijdelijk omlaag te latern gaan
*/
void gelijdelijkDownLoop();
/*
	Author		:	Joram van Leeuwen, Stef Ottenhof
	Description	:
		Functie om klauw terug omhoog te laten gaan
*/
void klauwOmhoog();
/*
	Author		:	Joram van Leeuwen, Stef Ottenhof
	Description	:
		Functies om klauw te openenen en te sluiten
*/
void klauwOpen();
void klauwDicht();
// Signal handler that will be called when Ctrl+C is pressed to stop the program
void exit_signal_handler(int signo);


#endif