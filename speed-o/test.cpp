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
	BP.set_motor_power(PORT_A, lspd+6);
	BP.set_motor_power(PORT_B, rspd);
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
    BP.reset_all();    // Reset everything so there are no run-away motors
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
		fwd(-6,0);
	}
}
