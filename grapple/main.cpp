#include "BrickPi3.h" // for BrickPi3
#include <stdio.h>      // for printf
#include <unistd.h>     // for usleep
#include <signal.h>     // for catching exit signals

BrickPi3 BP;

void exit_signal_handler(int signo);

/*
  Author:       Duur
  Description:  setSensors set all the sensors for a specific robot
                and immediatly sets them.
*/
void setSensors(){
  //BP.set_sensor_type();
}



int main(){
  while(true){
    sleep(5);
  }
  return 0;
}

// Signal handler that will be called when Ctrl+C is pressed to stop the program
void exit_signal_handler(int signo){
  if(signo == SIGINT){
    BP.reset_all();    // Reset everything so there are no run-away motors
    exit(-2);
  }
}