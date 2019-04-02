#include "BrickPi3.h"   // for BrickPi3
#include <iostream>     // voor printen
#include <stdio.h>      // for printf
#include <unistd.h>     // for usleep
#include <signal.h>     // for catching exit signals
#include <thread>       // thread

using namespace std;
BrickPi3 BP;

void exit_signal_handler(int signo);

bool battery = true;

void batteryLevel(void){
  //printf("Battery voltage : %.3f\n", BP.get_voltage_battery());
  while(true){
    if(BP.get_voltage_battery() <= 9.0){
      cout << "Yeeter de yoot de batterij is dood. T_T" << endl;
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