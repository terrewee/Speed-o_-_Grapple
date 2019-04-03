#include "BrickPi3.h"   // for BrickPi3
#include <iostream>     // for printf
#include <unistd.h>     // for usleep and sleep
#include <signal.h>     // for catching exit signals
#include <iomanip>		// for setw and setprecision

using namespace std;

BrickPi3 BP;

//sensor_ultrasonic_t Ultrasonic1;

void exit_signal_handler(int signo);

void exit_signal_handler(int signo)
{
if(signo == SIGINT)
    {
        BP.reset_all();
        exit(-2);
    }
}

void setSensors()
{
	BP.set_sensor_type(PORT_2,SENSOR_TYPE_NXT_ULTRASONIC);
}

void fwd(int lspd, int rspd)
{
//    lspd *= -1;
//    rspd *= -1;
    BP.set_motor_power(PORT_B, lspd);
    BP.set_motor_power(PORT_C, rspd);
}

/*
void gradualSpeedIncrease()
{
	sensor_ultrasonic_t Ultrasonic1;
    int currentSpeed = 0;
//    if(BP.get_sensor(PORT_2, Ultrasonic1) == 0)
//    {
        while(true)
        {
		if(BP.get_sensor(PORT_2, Ultrasonic1) == 0)
		{
            		if(Ultrasonic1.cm <= 20)
            		{
                		fwd(0,0);
                		break;
            		}
            		currentSpeed += 5;
            		fwd(currentSpeed, currentSpeed);
            		cout << "Current speed: " << currentSpeed << endl;
            		sleep(5);
        	}
    		else
    		{
        		cout << "Error - no sensor.";
    		}
	}
}
*/

void readSensor()
{
	int currentSpeed = 100;
	sensor_ultrasonic_t Ultrasonic1;
	while(true)
	{
		if(BP.get_sensor(PORT_2, Ultrasonic1) == 0)
		{
//			cout << "Ultrasonic sensor in cm: " << Ultrasonic1.cm << endl;
//			sleep(2);
			//if(currentSpeed >= 80){fwd(0,0); break;}
			fwd(currentSpeed, currentSpeed);
			//cout << currentSpeed << endl;
			//sleep(1);
			//currentSpeed += 5;
		}
	}
}

int main()
{
    	signal(SIGINT, exit_signal_handler);
    	BP.detect();
	setSensors();
	//gradualSpeedIncrease();
	readSensor();
}

