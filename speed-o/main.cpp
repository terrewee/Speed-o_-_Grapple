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
	Author		  :	Joram van Leeuwen, Duur Alblas
	Description :
		          Code om de waardes van geëncodeerde motors uit te lezen
*/

void readEncodedMotor()
{
	BP.offset_motor_encoder(PORT_A, BP.get_motor_encoder(PORT_A));
	BP.offset_motor_encoder(PORT_B, BP.get_motor_encoder(PORT_B));
	BP.offset_motor_encoder(PORT_C, BP.get_motor_encoder(PORT_C));
	BP.offset_motor_encoder(PORT_D, BP.get_motor_encoder(PORT_D));
	while(true)
	{
		int32_t encodedA = BP.get_motor_encoder(PORT_A);
		int32_t encodedB = BP.get_motor_encoder(PORT_B);
		int32_t encodedC = BP.get_motor_encoder(PORT_C);
		int32_t encodedD = BP.get_motor_encoder(PORT_D);
	    	cout << "Motor A: " << endl << "Encoded: " << encodedA << endl << "--------------------------------" << endl;
		cout << "Motor B: " << endl << "Encoded: " << encodedB << endl << "--------------------------------" << endl;
		cout << "Motor C: " << endl << "Encoded: " << encodedC << endl << "--------------------------------" << endl;
		cout << "Motor D: " << endl << "Encoded: " << encodedD << endl << "--------------------------------" << endl << endl;
		sleep(2);
	}

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
	readEncodedMotor();
}
