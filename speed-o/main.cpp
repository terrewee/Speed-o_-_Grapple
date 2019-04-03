#include "BrickPi3.h" // for BrickPi3
#include <stdio.h>      // for printf
#include <unistd.h>     // for usleep
#include <signal.h>     // for catching exit signals

BrickPi3 BP;

/*
	Author 		: 	Duur Alblas
	Description :
		Short code to power up 2 motors using 2 variables.
*/
void fwd(int lspd , int rspd){
  lspd *=-1;
  rspd *=-1;
  BP.set_motor_power(PORT_B, lspd);
  BP.set_motor_power(PORT_C, rspd);
}

/*
	Author		:	Gerjan Dons
	Description :
		Small code to make the robot move
*/
void drive(){
  int32_t lspd = 0;
  int32_t rspd = 0;
  while (true){
    cout << "Geef snelheid op : " << endl;
    getline(cin, lspd);
    rspd = lspd;
    fwd(lspd,rspd);
  }
}


bool battery = true;    //battery level function
/* 
  Author:       Maaike & Duur
  Description:  Bateryscheck which changes the 
                global bool battery to false if battery is low
*/


void batteryLevel(void){
  //printf("Battery voltage : %.3f\n", BP.get_voltage_battery());
  while(true){
    if(BP.get_voltage_battery() <= 9.0){
      cout << "de batterij is dood. T_T" << endl;
      ::battery = false;
    }
    else{
      ::battery = true;
    }
    sleep(5);
  }
}


int main(){
  thread checkBattery (batteryLevel);
  drive()
  return 0;
}

// Signal handler that will be called when Ctrl+C is pressed to stop the program
void exit_signal_handler(int signo){
  if(signo == SIGINT){
    BP.reset_all();    // Reset everything so there are no run-away motors
    exit(-2);
  }
}