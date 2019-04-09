#include "BrickPi3.h"   // for BrickPi3
#include <iostream>     // for printf
#include <unistd.h>     // for usleep and sleep
#include <signal.h>     // for catching exit signals
#include <iomanip>	// for setw and setprecision

using namespace std;

BrickPi3 BP;

void exit_signal_handler(int signo);

/*
	Author 		:	Duur Alblas
	Description	:
		Initialize all sensors
*/
void setSensors()
{
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
	Author		:	Joram van Leeuwen
	Description	:
		Functie om arm over kantelpunt te krijgen
*/
void brengNaarKantelPunt()
{
	BP.set_motor_limits(PORT_A, 40, 0);
	encodeMotor(-50);
}

/*
	Author		:	Joram van Leeuwen, Stef Ottenhof
	Description	:
		Functie om klauw gelijdelijk omlaag te latern gaan
*/
void gelijdelijkDownLoop()
{
	int32_t encoder = -50;
	while(encoder > -110)
	{
		encodeMotor(-5);
		usleep(500000);
		encoder = encoder - 5;
	}
}

/*
	Author		:	Joram van Leeuwen, Stef Ottenhof
	Description	:
		Functie om klauw terug omhoog te laten gaan
*/
void klauwOmhoog()
{
	BP.set_motor_limits(PORT_A, 50, 0);
	encodeMotor(130);	// zelfde als totale neerwaartse beweging
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
	sleep(1);
	brengNaarKantelPunt();
	sleep(1);
	gelijdelijkDownLoop();
	sleep(1);
	klauwDicht();
	sleep(1);
	klauwOmhoog();
	sleep(3);
	BP.set_motor_power(PORT_A, 0);
}
