#include "BrickPi3.h"   // for BrickPi3                                                     //TE DOEN
#include <iostream>     // for printf
#include <unistd.h>     // for usleep and sleep
#include <signal.h>     // for catching exit signals
#include <iomanip>	// for setw and setprecision

using namespace std;

BrickPi3 BP;

void exit_signal_handler(int signo);

/*
	Author		:	Joram van Leeuwen
	Description	:
		Simpele code om twee motoren eenmaal aan te sturen.
*/
void fwd(int lspd , int rspd)
{
	BP.set_motor_power(PORT_B, lspd);
	BP.set_motor_power(PORT_C, rspd);
}

/*
	Author 		:	Duur Alblas
	Description	:
		Initialize all sensors
*/
void setSensors(){
	BP.set_sensor_type(PORT_1,SENSOR_TYPE_NXT_COLOR_FULL);
	BP.set_sensor_type(PORT_2,SENSOR_TYPE_NXT_ULTRASONIC);
	BP.set_sensor_type(PORT_3,SENSOR_TYPE_NXT_LIGHT_ON);
	BP.set_sensor_type(PORT_4,SENSOR_TYPE_NXT_COLOR_FULL);
}
/*
	Author:		:	Joram van Leeuwen, Duur Alblas
	Description	:
		Short code to set a single motor encoder.
*/
void encodeMotor(int32_t pos)
{
	BP.set_motor_position_relative(PORT_A, pos);
}

/*
	Author		:	Joram van Leeuwen, Stef Ottenhof
	Description	:
		Functie om klauw gelijdelijk omlaag te latern gaan
*/
void gelijdelijkDownLoop()
{
	BP.set_motor_limits(PORT_A, 30, 0);
	int counter = 0;
	encodeMotor(-60);
	while(counter < 10)
	{
		sleep(1);
		encodeMotor(-3);
		counter++;

	}
}

/*
	Author		:	Joram van Leeuwen, Stef Ottenhof
	Description	:
		Functie om klauw terug omhoog te laten gaan
*/
void klauwOmhoog()
{
	BP.set_motor_limits(PORT_A, 45, 0);	// speed 20 als limiet voor opwaartse beweging.
<<<<<<< HEAD
	encodeMotor(100);			// rotatie is ~110.
=======
	encodeMotor(110);			// rotatie is ~110.
>>>>>>> ea00600bd9a489eec0c9cd4c17b84f97241de52a
}

/*
	Author		:	Joram van Leeuwen, Stef Ottenhof
	Description	:
		Functies om klauw te openenen en te sluiten
*/
void klauwOpen()
{
	BP.set_motor_limits(PORT_D, 60, 0);
	BP.set_motor_position_relative(PORT_D, -180);
}
void klauwDicht()
{
	BP.set_motor_limits(PORT_D, 60, 0);
	BP.set_motor_position_relative(PORT_D, 180);
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
	setSensors();
	klauwOpen();
	gelijdelijkDownLoop();
	sleep(1);
	klauwDicht();
	sleep(1);
	klauwOmhoog();
}
