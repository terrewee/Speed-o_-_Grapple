#include "BrickPi3.h"   // for BrickPi3
#include <iostream>     // for printf
#include <unistd.h>     // for usleep and sleep
#include <signal.h>     // for catching exit signals
#include <iomanip>		// for setw and setprecision
#include <string>		// for strings

using namespace std;

BrickPi3 BP;

void exit_signal_handler(int signo);

/*
-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	Author		:	Duur Alblas
	Description :
		Short code to set motor encoders.
*/
void encodeMotors(int32_t lpos , int32_t rpos)
{
	BP.set_motor_position_relative(PORT_B, lpos);
	BP.set_motor_position_relative(PORT_C, rpos);
}

/*
	Author		:	Joram van Leeuwen, Duur Alblas
	Description :
		Code om de waardes van geëncodeerde motors uit te lezen
*/

void readEncodedMotor(char motor)
{
	string port = "PORT_" + motor;
	BP.offset_motor_encoder(port, BP.get_motor_encoder(port));
	int32_t encoded = BP.get_motor_encoder(port);

    cout << "Motor: " << motor << endl;
   	cout << "Encoded: " << encoded << endl;
}

void exit_signal_handler(int signo)
{
if(signo == SIGINT)
    {
        BP.reset_all();
        exit(-2);
    }
}

int main()
{
	signal(SIGINT, exit_signal_handler);
  	BP.detect();
	char dePort;
	cout << "Programma om geëncodeerde motors te testen" << endl << "Geef de port van de motor [A/B/C/D]: ";
	cin >> dePort;
	if(dePort != 'A' || dePort != 'B' || dePort != 'C' || dePort != 'D'){exit;}
	while(true)
	{
		readEncodedMotor(dePort);
	}
}