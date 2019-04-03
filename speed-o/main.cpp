#include "BrickPi3.h"   // for BrickPi3                                                     //TE DOEN
#include <iostream>     // for printf
#include <unistd.h>     // for usleep and sleep
#include <signal.h>     // for catching exit signals
#include <iomanip>		// for setw and setprecision

using namespace std;

sensor_color_t Color1;                                                                      //check de ports voor cohesie in sensornamen
sensor_ultrasonic_t Ultrasonic2;
sensor_touch_t Touch3;
sensor_light_t Light4;

BrickPi3 BP;

void exit_signal_handler(int signo);

void fwd(int lspd , int rspd)
{
	BP.set_motor_power(PORT_B, lspd);
	BP.set_motor_power(PORT_C, rspd);
}

/*
	Author		:	Joram van Leeuwen, Duur Alblas
	Description :
		Short code to set a single motor encoder.
*/

/*
	Author		:	Joram van Leeuwen
	Description :
		Code om een motor encoder te lezen -- alweer.
*/
void readEncoder()
{
	BP.offset_motor_encoder(PORT_A, BP.get_motor_encoder(PORT_A));
	int32_t encoderA = BP.get_motor_encoder(PORT_A);
	cout << "Motor A: " << endl << encoderA << endl;
}

void encodeMotor(int32_t pos)
{
	BP.set_motor_position_relative(PORT_A, pos);
}

/*
    Author: Joram van Leeuwen
    Description: Grijpt een object in de klauw indien de touch sensor iets aanraakt. Dit is een puur iets waarvan ik niet zeker ben of het werkt
*/
void klauwNaarBeneden()
{
	BP.set_motor_limits(PORT_A, 5, 0); 	// speed 5 als limiet voor het naar beneden gaan.
	encodeMotor(-110);			// rotatie is ~110. Negatief voor neerwaarts.
}

void klauwOmhoog()
{
	BP.set_motor_limits(PORT_A, 20, 0);	// speed 20 als limiet voor opwaartse beweging.
	encodeMotor(110);			// rotatie is ~110.
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
	//setSensors();
	klauwNaarBeneden();
	sleep(10);				// sleep nu op 10 - pas aan zoals noodzakelijk is.
	klauwOmhoog();
}
