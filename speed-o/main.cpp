#include "BrickPi3.h" // for BrickPi3
#include <stdio.h>      // for printf
#include <unistd.h>     // for usleep
#include <signal.h>     // for catching exit signals
#include <iostream>     // for printf

using namespace std;

BrickPi3 BP;

/*
	Author 		: 	Duur Alblas
	Description :
		Short code to power up 2 motors using 2 variables.
*/
void fwd(int lspd , int rspd){
  lspd *=-1;
  rspd = (rspd *10)/13;
  BP.set_motor_power(PORT_B, lspd);
  BP.set_motor_power(PORT_C, rspd);
}

/*
	Author		:	Gerjan Dons
	Description :
		Small code to make the robot move
*/
void drive(){
  int32_t lspd = 100;
  int32_t rspd = 100;
  /*
  while (true){
    cout << "Geef snelheid op : " << endl;
    getline(cin, lspd);
    rspd = lspd;
    fwd(lspd,rspd);
  }
   */
}


// Signal handler that will be called when Ctrl+C is pressed to stop the program
void exit_signal_handler(int signo){
  if(signo == SIGINT){
    BP.reset_all();    // Reset everything so there are no run-away motors
    exit(-2);
  }
}

int main(){
    signal(SIGINT, exit_signal_handler); // register the exit function for Ctrl+C
    drive()
    return 0;
}