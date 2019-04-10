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

bool crossroaddetectie(sensor_color_t Color2, sensor_color_t Color4){
  ::crossroad = 0;
  while (true){
    if((BP.get_sensor(PORT_2, Color2) == 0)&&(BP.get_sensor(PORT_4, Color4) == 0)){

      if (Color2.color == 1 && Color4.color == 1){ usleep(0.5); ::crossroad++;}//sleep zodat hij niet hetzelfde kruispunt 2 keer registreert
      if (Color2.color == 1 && Color4.color == 1){ usleep(0.5); ::crossroad++;}//sleep zodat hij niet hetzelfde kruispunt 2 keer registreert
      else if (Color2.color == 1 && Color4.color == 0){usleep(0.5); ::crossroad++;}
      else if (Color2.color == 0 && Color4.color == 1){ usleep(0.5); ::crossroad++;}
    }
  }
}

//Turns the robot to the right, and updates the value of GP.direction.
void turnLeft(gridPoints & GP){
  int lpos = -250; 		//Left position
  int rpos = 250;		//Right position
  int32_t EncoderB = BP.get_motor_encoder(PORT_B);
  int32_t EncoderC = BP.get_motor_encoder(PORT_C);
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
}

//Turns the rorbot to the left, and updates the value of GP.direction.
void turnRight(gridPoints & GP){
  int lpos = 250;		//Left position
  int rpos = -250;		//Right position
  int32_t EncoderB = BP.get_motor_encoder(PORT_B);
  int32_t EncoderC = BP.get_motor_encoder(PORT_C);
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
}

void setSensors(){
	BP.set_sensor_type(PORT_1,SENSOR_TYPE_NXT_COLOR_FULL);
	BP.set_sensor_type(PORT_2,SENSOR_TYPE_NXT_ULTRASONIC);
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
        int Tp = 90;
        int Kp = 1;

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
          error = ((lightvalue-600)/100)+30 - offset;

          Turn = error * Kp;
          Turn = Turn/1;

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
	setSensors();
	signal(SIGINT, exit_signal_handler); 
	BP.detect();

 	while(true){
		followLine();
	}
}

