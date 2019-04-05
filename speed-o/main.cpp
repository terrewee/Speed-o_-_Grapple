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
		Twee functies voor het omhoog en omlaag halen van de klauw.
*/
void klauwNaarBeneden()
{
	int draai;
	int kracht;
	while(true)
	{
	BP.set_motor_limits(PORT_A, 25, 0); 	// speed 5 als limiet voor het naar beneden gaan.
	cout << "Draai keer 1: ";
	cin >> draai;
	encodeMotor(draai);			// rotatie is ~110. Negatief voor neerwaarts.
	// klauw openen hier
	cout << "Draai keer 2: ";
	cin >> draai;
	cout << "Kracht keer 2: ";
	cin >> kracht;
	BP.set_motor_limits(PORT_A, kracht, 0);
	encodeMotor(draai);
	sleep(1);
	encodeMotor(20);
	// klauw sluiten hier
	}
}

void klauwNaarBenedenKantelpunt()
{
	int uChoice = 0;
	int draai;
	while(uChoice != -1)
	{
		cout << "Draai: ";
		cin >> draai;
		BP.set_motor_limits(PORT_A, 80, 0);
		encodeMotor(draai);
		cout << "1. Verder \n-1. Stop";
		cin >> uChoice;
	}
}

void klauwDownTest(int startPos)
{
	int32_t huidigePos = 0;
	BP.offset_motor_encoder(PORT_A, BP.get_motor_encoder(PORT_A));
	while(huidigePos > startPos-90)
	{
		BP.set_motor_power(PORT_A, 3);
		huidgePos = BP.offset_motor_encoder(PORT_A, BP.get_motor_encoder(PORT_A));
	}
	encodeMotor(0);
}

void klauwOmhoog()
{
	BP.set_motor_limits(PORT_A, 40, 0);	// speed 20 als limiet voor opwaartse beweging.
	encodeMotor(110);			// rotatie is ~110.
}

void klauwOpen()
{
	BP.set_motor_limits(PORT_A, 10, 0);
	encodeMotor(-160);
}

void klauwDicht()
{
	BP.set_motor_limits(PORT_A, 10, 0);
	encodeMotor(160);
}

void exit_signal_handler(int signo)
{
if(signo == SIGINT)
    {
        BP.reset_all();
        exit(-2);
    }
}

void testValues()
{
	int limiet;
	int kracht;
	while(true)
	{
		cout << "Limiet: ";
		cin >> limiet;
		cout << "Draai: ";
		cin >> kracht;

		BP.set_motor_limits(PORT_A, limiet, 0);
		encodeMotor(kracht);
	}
}

int main()
{
	signal(SIGINT, exit_signal_handler);
  	BP.detect();
	setSensors();
	BP.offset_motor_encoder(PORT_A, BP.get_motor_encoder(PORT_A));
	int startPos = BP.get_motor_encoder(PORT_A);
	while(true)
	{
		klauwNaarBenedenKantelpunt();
		klauwDownTest(startPos);
	}
//	klauwNaarBeneden();
//	cout << "Klaar met naar beneden" << endl;
//	sleep(5);
//	klauwOmhoog();
//	testValues();
}
