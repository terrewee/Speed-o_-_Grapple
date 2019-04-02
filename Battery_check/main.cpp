#include "BrickPi3.h" // for BrickPi3
#include <stdio.h>      // for printf
#include <unistd.h>     // for usleep
#include <signal.h>     // for catching exit signals

BrickPi3 BP;

void exit_signal_handler(int signo);

void batteryLevel(void){
  printf("Battery voltage : %.3f\n", BP.get_voltage_battery());
  printf("9v voltage      : %.3f\n", BP.get_voltage_9v());
  printf("5v voltage      : %.3f\n", BP.get_voltage_5v());
  printf("3.3v voltage    : %.3f\n", BP.get_voltage_3v3());
}


int main(){
  batteryLevel();
  return 0;
}

// Signal handler that will be called when Ctrl+C is pressed to stop the program
void exit_signal_handler(int signo){
  if(signo == SIGINT){
    BP.reset_all();    // Reset everything so there are no run-away motors
    exit(-2);
  }
}