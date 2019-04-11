#include "BrickPi3.h"     // for BrickPi3
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
#include <vector>

using namespace std;

BrickPi3 BP;
void exit_signal_handler(int signo);

bool running = true;
int crossroad;

void setSensors(){
	BP.set_sensor_type(PORT_2,SENSOR_TYPE_NXT_COLOR_FULL);
	BP.set_sensor_type(PORT_1,SENSOR_TYPE_NXT_ULTRASONIC);
	BP.set_sensor_type(PORT_3,SENSOR_TYPE_NXT_LIGHT_ON);
	BP.set_sensor_type(PORT_4,SENSOR_TYPE_NXT_COLOR_FULL);
}

void crossroaddetectie()
{
	sensor_color_t      Color2;
	sensor_color_t      Color4;
	::crossroad = 0;
	while (::running)
	{
		if((BP.get_sensor(PORT_2, Color2) == 0) && (BP.get_sensor(PORT_4, Color4) == 0))
		{
			if (Color2.color == 1 || Color4.color == 1)
			{
				usleep(200000); // sleep van 100 ms zodat hetzelfde kruispunt niet tweemaal wordt geregistreerd
				::crossroad++;	// increment globale variabele crossroads gezien
			}
		}
		//sleep(0.5);
		cout << "2: " << (int) Color2.color << " 4: " << (int) Color4.color << endl;
		cout << "Crossroad number: " << ::crossroad << endl;
	}
}

struct routeCount {
  vector<char> direction = {};
  vector<int> amount = {};
};

routeCount initRouteCount(const string & myRoute) {
  routeCount tStruct;
  tStruct.direction.push_back(' ');
  tStruct.amount.push_back(0);
  int sIndex = 0;
  for(char direction : myRoute){
    if (tStruct.direction[sIndex] == direction) tStruct.amount[sIndex]++;
    else if (tStruct.direction[sIndex] == ' ') {tStruct.direction[sIndex] = direction; tStruct.amount[sIndex]++;}
    else {tStruct.direction.push_back(direction); tStruct.amount.push_back(1); sIndex++;}
  }
  return tStruct;
}

void draaiLinks()
{
	BP.get_motor_encoder(PORT_B);
	BP.get_motor_encoder(PORT_C);
	BP.set_motor_position_relative(PORT_B, 116);
	BP.set_motor_position_relative(PORT_C, -116);
}

void resetMotors(){
	BP.set_motor_power(PORT_B, 0);
	BP.set_motor_power(PORT_C, 0);
}

void moveForward(int lspd, int rspd){
	BP.set_motor_power(PORT_B,-lspd);
	BP.set_motor_power(PORT_C,-rspd);
}

void draaiRechts(){
	sensor_light_t Light3;
	while(true){
        if(BP.get_sensor(PORT_3, Light3) == 0){
            if(Light3.reflected < 2300){
                break;
            }
            moveForward(20, 5);
        }
	}
	resetMotors();
}

bool stopVoorObject()
{
	sensor_ultrasonic_t Ultrasonic1;
	if(BP.get_sensor(PORT_1, Ultrasonic1) == 0)
	{
		if(Ultrasonic1.cm <= 20)
		{
			return true;
		}
		return false;
	}
}

void followLine(int aantalKeerTeGaan) // aantalKeerTeGaan = aantal keer dat de scout 1 kant op moet
{

        sensor_light_t Light3;

        int offset = 45;
        int Tp = 25;
        int Kp = 2;

        int lastError = 0;
        int Turn = 0;
        int lightvalue = 0;
        int error = 0;

        int lspd = 0;
        int rspd = 0;
		while(true)
		{
			if(BP.get_sensor(PORT_3, Light3) == 0){
			cout << "crossroad: " << ::crossroad << endl;
			if(::crossroad == aantalKeerTeGaan - 1)
			{
//				Tp = 10;
//				Kp = 1;
			}
			else if(::crossroad == aantalKeerTeGaan)
			{
				resetMotors();
				break;
			}
			lightvalue = Light3.reflected;
			error = ((lightvalue-1700)/40)+30 - offset;

			Turn = error * Kp;
			Turn = Turn/1;

			lspd = Tp + Turn;
			rspd = Tp - Turn;

			if(stopVoorObject() == true)
			{
				resetMotors();
				sleep(1);
			}

			if(::crossroad == aantalKeerTeGaan - 1)
			{
				lspd = lspd / 2;
				rspd = rspd / 2;
			}
			moveForward(lspd,rspd);
			lastError = error;
			cout << "lspd: " << lspd << endl << "rspd: " << rspd << endl;
		}
        }
}

void exit_signal_handler(int signo){
  if(signo == SIGINT){
    BP.reset_all();    // Reset everything so there are no run-away motors
	::running = false;
    exit(-2);
  }
}

int main()
{
	signal(SIGINT, exit_signal_handler); // register the exit function for Ctrl+C
	BP.detect();
	BP.reset_all();
	for (int i = 0; i < 5; ++i)
	{
		cout << ".";
		if (i == 3)
		{
			setSensors();
		}
    		sleep(1);
  	}
	cout << endl << "Initialized" << endl;

	thread kruispunt (crossroaddetectie);

 	followLine(2);	// 2 voor testje -- pas dit dus aan met de mee te geven parameter
	draaiRechts();
	BP.reset_all();

	kruispunt.join();
}

