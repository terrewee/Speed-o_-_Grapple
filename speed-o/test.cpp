#include "BrickPi3.h"   // for BrickPi3
#include <iostream>     // for printf
#include <unistd.h>     // for usleep and sleep
#include <signal.h>     // for catching exit signals
#include <iomanip>	// for setw and setprecision
#include <thread>

using namespace std;

BrickPi3 BP;

void fwd(int lspd , int rspd)
{
	lspd = lspd - 3;
	BP.set_motor_power(PORT_A, lspd);
	BP.set_motor_power(PORT_B, rspd);
}

void readEncodedMotor()
{
	BP.offset_motor_encoder(PORT_A, BP.get_motor_encoder(PORT_A));
	BP.offset_motor_encoder(PORT_B, BP.get_motor_encoder(PORT_B));
	usleep(2500000);
	int32_t encodedA = BP.get_motor_encoder(PORT_A);
	int32_t encodedB = BP.get_motor_encoder(PORT_B);
    	cout << "Motor A: " << endl << "Encoded: " << encodedA << endl << "--------------------------------" << endl;
	cout << "Motor B: " << endl << "Encoded: " << encodedB << endl << "--------------------------------" << endl;
}

void setSensors()
{
	BP.set_sensor_type(PORT_1,SENSOR_TYPE_NXT_COLOR_FULL);
	BP.set_sensor_type(PORT_2,SENSOR_TYPE_NXT_ULTRASONIC);
	BP.set_sensor_type(PORT_3,SENSOR_TYPE_NXT_LIGHT_ON);
	BP.set_sensor_type(PORT_4,SENSOR_TYPE_NXT_COLOR_FULL);
}

void exit_signal_handler(int signo);

void exit_signal_handler(int signo){
  if(signo == SIGINT){
    BP.reset_all();
    exit(-2);
  }
}

int main()
{
	signal(SIGINT, exit_signal_handler);
	BP.detect();
	int motorA;
	int motorB;
	while(true)
	{
		cout << "Motor A power: ";
		cin >> motorA;
		cout << "Motor B power: ";
		cin >> motorB;
		fwd(motorA, motorB);
		sleep(3);
//		fwd(-6,0);
		//sleep(3);
		//fwd(-6,0);
		readEncodedMotor();
	}
}
