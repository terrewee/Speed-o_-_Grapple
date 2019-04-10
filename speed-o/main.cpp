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

using namespace std;

BrickPi3 BP;
void exit_signal_handler(int signo);

int crossroad;

void crossroaddetectie(){
  sensor_color_t      Color2;
  sensor_color_t      Color4;
  ::crossroad = 0;
  while (true){
    if((BP.get_sensor(PORT_2, Color2) == 0)&&(BP.get_sensor(PORT_4, Color4) == 0)){
      if (Color2.color == 1 || Color4.color == 1){ sleep(0.01); ::crossroad++;}//sleep zodat hij niet hetzelfde kruispunt 2 keer registreert
    }
  sleep(0.5);
  cout << "2: " << (int) Color2.color << " 4: " << (int) Color4.color << endl;
  cout << "Crossroad number: " << ::crossroad << endl;
  }
}

//Turns the robot to the right, and updates the value of GP.direction.
void turnLeft(int lpos, int rpos){	// zet gridpoints GP er weer in
  int32_t EncoderB = BP.get_motor_encoder(PORT_B);
  int32_t EncoderC = BP.get_motor_encoder(PORT_C);
/*
  if(GP.direction == 'n'){
    GP.direction = 'w';
		BP.set_motor_position_relative(PORT_B, lpos);
		BP.set_motor_position_relative(PORT_C, rpos);
		sleep(1);
		resetMotorsAB();
  }
  else if(GP.direction == 'w'){
    GP.direction = 's';
		BP.set_motor_position_relative(PORT_B, lpos);
		BP.set_motor_position_relative(PORT_C, rpos);
		sleep(1);
		resetMotorsAB();
  }
  else if(GP.direction == 's'){
    GP.direction = 'e';
		BP.set_motor_position_relative(PORT_B, lpos);
		BP.set_motor_position_relative(PORT_C, rpos);
		sleep(1);
		resetMotorsAB();
  }
  else{
    GP.direction = 'n';
		BP.set_motor_position_relative(PORT_B, lpos);
		BP.set_motor_position_relative(PORT_C, rpos);
		sleep(1);
		resetMotorsAB();
  }
*/
	BP.set_motor_position_relative(PORT_B, lpos);
	BP.set_motor_position_relative(PORT_C, rpos);
}

//Turns the rorbot to the left, and updates the value of GP.direction.
void turnRight(int lpos, int rpos){	// zet gridpoints GP er weer in
  int32_t EncoderB = BP.get_motor_encoder(PORT_B);
  int32_t EncoderC = BP.get_motor_encoder(PORT_C);
/*
  if(GP.direction == 'n'){
    GP.direction = 'e';
		BP.set_motor_position_relative(PORT_B, lpos);
		BP.set_motor_position_relative(PORT_C, rpos);
		sleep(1);
		resetMotorsAB();
  }
  else if(GP.direction == 'e'){
    GP.direction = 's';
		BP.set_motor_position_relative(PORT_B, lpos);
		BP.set_motor_position_relative(PORT_C, rpos);
		sleep(1);
		resetMotorsAB();
  }
  else if(GP.direction == 's'){
    GP.direction = 'w';
		BP.set_motor_position_relative(PORT_B, lpos);
		BP.set_motor_position_relative(PORT_C, rpos);
		sleep(1);
		resetMotorsAB();
  }
  else{
    GP.direction = 'n';
		BP.set_motor_position_relative(PORT_B, lpos);
		BP.set_motor_position_relative(PORT_C, rpos);
		sleep(1);
		resetMotorsAB();
  }
*/
	BP.set_motor_position_relative(PORT_B, lpos);
	BP.set_motor_position_relative(PORT_C, rpos);
}


void setSensors(){
  BP.set_sensor_type(PORT_1,SENSOR_TYPE_NXT_ULTRASONIC);
	BP.set_sensor_type(PORT_2,SENSOR_TYPE_NXT_COLOR_FULL);
	BP.set_sensor_type(PORT_3,SENSOR_TYPE_NXT_LIGHT_ON);
	BP.set_sensor_type(PORT_4,SENSOR_TYPE_NXT_COLOR_FULL);
}

void resetMotors(){
	BP.set_motor_power(PORT_B, 0);
	BP.set_motor_power(PORT_C, 0); 
}

void moveForward(int lspd, int rspd){
	BP.set_motor_power(PORT_B,-lspd);
	BP.set_motor_power(PORT_C,-rspd);
}

void followLine()
{
        sensor_light_t Light3;

        int offset = 45;
        int Tp = 40;
        int Kp = 5;

        int lastError = 0;
        int Turn = 0;
        int lightvalue = 0;
        int error = 0;

        int lspd = 0;
        int rspd = 0;

        if (BP.get_sensor(PORT_3, Light3) == 0)
        {
          lightvalue = Light3.reflected;
//	cout << "Lichtwaarde: " << lightvalue << endl;
          error = ((lightvalue-1600)/50)+30 - offset;

          Turn = error * Kp;
          Turn = Turn/3;

          lspd = Tp + Turn;
          rspd = Tp - Turn;

          moveForward(lspd,rspd);

          lastError = error;
        }
}

void exit_signal_handler(int signo){
  if(signo == SIGINT){
    BP.reset_all();    // Reset everything so there are no run-away motors
    exit(-2);
  }
}

int main(){
  signal(SIGINT, exit_signal_handler); // register the exit function for Ctrl+C
  BP.detect();
  BP.reset_all();
  for (int i = 0; i < 5; ++i){
    cout << ".";
    if (i == 3){
      setSensors();
    }
    sleep(1);
  }
  cout << endl << "Initialized" << endl;
	
  sensor_light_t      Light3;
  

  thread kruispunt (crossroaddetectie);

 	while(true){
/*		int lpos;
		int rpos;
		cout << "lpos: ";
		cin >> lpos;
		cout << "rpos: ";
		cin >> rpos;
		turnRight(lpos, rpos);
*/
//		if(BP.get_sensor(PORT_3, Light3) == 0){cout << "Ligt value: " <<  (int)Light3.reflected << endl; sleep(2);}
		followLine();
    
	}
}

