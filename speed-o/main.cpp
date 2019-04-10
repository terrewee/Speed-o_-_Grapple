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

bool crossroaddetectie(const sensor_color_t & Color2, const sensor_color_t & Color4){
  ::crossroad = 0;
  while (true){
    if((BP.get_sensor(PORT_2, Color2) == 0)&&(BP.get_sensor(PORT_4, Color4) == 0)){
      if      (Color2.color == 1 && Color4.color == 1){ usleep(0.5); ::crossroad++;}    //sleep zodat hij niet hetzelfde kruispunt 2 keer registreert
      else if (Color2.color == 1 && Color4.color == 0){ usleep(0.5); ::crossroad++;}
      else if (Color2.color == 0 && Color4.color == 1){ usleep(0.5); ::crossroad++;}
    }
  }
}

void followLine()
{
        sensor_light_t Light3;

        int offset = 45;
        int Tp = 25;
        int Kp = 5;

        lastError = 0;
        int Turn = 0;
        int lightvalue = 0;
        int error = 0;

        int lspd = 0;
        int rspd = 0;

        if (BP.get_sensor(PORT_3, Light3) == 0)
        {
                lightvalue = Light3.reflected;
        }
        error = ((lightvalue-1400)/60)+30 - offset;

        Turn = error * Kp;
        Turn = Turn/1;

        lspd = Tp + Turn;
        rspd = Tp - Turn;

        moveForward();

        lastError = error;
}

void exit_signal_handler(int signo){
  if(signo == SIGINT){
    BP.reset_all();    // Reset everything so there are no run-away motors
    exit(-2);
  }
}

int main(){
	signal(SIGINT, exit_signal_handler); 
	BP.detect();

 	while(true){
		followline();
	}
}

