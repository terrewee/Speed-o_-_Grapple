#include "BrickPi3.h" // for BrickPi3
#include <stdio.h>      // for printf
#include <unistd.h>     // for usleep
#include <signal.h>     // for catching exit signals
#include <iostream>     //voor cout enz.
#include <thread>       //thread

BrickPi3 BP;
using namespace std;
void setSensors(){
  BP.set_sensor_type(PORT_1, SENSOR_TYPE_NXT_ULTRASONIC);
}

void encodeHead(int32_t pos){
    BP.set_motor_position_relative(PORT_D,pos);
}

void exit_signal_handler(int signo);
bool battery = true;    //battery level function
/*
  Author:       Mathilde, Maaike
  Description:  Functie voor het detecteren van obstakels en de
                ontwijking hiervan.
*/

struct range {
  bool obstakelInRangeLeft = false;
  bool obstakelInRangeRight = false;
  bool obstakelInRangeForward = false;
};
range obstakel;

int measure(sensor_ultrasonic_t Ultrasonic){
  cout << "beep boop" << endl;
  int ding = BP.get_sensor(PORT_1, Ultrasonic);
  cout << ding << endl;
  if(ding == 0){
    cout << "Ultrasonic sensor (S4): "   << Ultrasonic.cm << "cm" << endl;
  }
  sleep(5);
  return Ultrasonic.cm;
}

void stopHead(){
  //reset de motor dps
  BP.set_motor_dps(PORT_D, 0);
}
void lookLeft(sensor_ultrasonic_t Ultrasonic){
    //spin left
    encodeHead(-90);

    sleep(1);
    int waarde = measure(Ultrasonic);
    if (waarde <= 10.0){obstakel.obstakelInRangeLeft = true;}
    else{obstakel.obstakelInRangeLeft = false;}

    //reset to middle
    sleep(1);
    encodeHead(90);
}

void lookRight(sensor_ultrasonic_t Ultrasonic){
    //spin right.
    encodeHead(90);

    sleep(1);
    int waarde = measure(Ultrasonic);
    if (waarde <= 10.0){ obstakel.obstakelInRangeLeft = true;}
    else{obstakel.obstakelInRangeLeft = false;}

    //reset to middle
    sleep(1);
    encodeHead(-90);
} 

void lookForward(sensor_ultrasonic_t Ultrasonic){
  float waarde = measure(Ultrasonic);
  if (waarde <= 10.0){obstakel.obstakelInRangeForward = true;}
  else{obstakel.obstakelInRangeForward = false;}
  
}

void obstakelDetectie(sensor_ultrasonic_t Ultrasonic){
  while(true){
  lookForward(Ultrasonic);
  //main van obstakel
    stopHead();
    while (obstakel.obstakelInRangeForward == true){
      stopHead();
      
      cout << "LEFT " << endl;
      lookLeft(Ultrasonic);
      if (obstakel.obstakelInRangeLeft == true){cout << "cant go left" << endl;}
      else{cout << "can go left" << endl;}
      sleep (1);
      
      cout << "RIGHT " << endl;
      lookRight(Ultrasonic);
      if (obstakel.obstakelInRangeRight == true){cout << "cant go right" << endl;}
      else{cout << "can go right" << endl;}
      sleep (1);
      lookForward(Ultrasonic);
      if(obstakel.obstakelInRangeForward == true && obstakel.obstakelInRangeLeft == true && obstakel.obstakelInRangeRight == true){cout <<"Go back" << endl;} 
    }
  }
}


/*
  Author:       Maaike & Duur
  Description:  Bateryscheck which changes the
                global bool battery to false if battery is low
*/
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

/*
  Author:       Maaike & Duur
  Description:  Asks the user to supply a port and a sensor type to check the output
                of said function for a certain amount of time.
*/
void checkSensor(){
  sensor_color_t        Color;
  sensor_ultrasonic_t   Ultrasonic;
  sensor_light_t        Light;
  sensor_touch_t        Touch;

  int portNr = 0;
  uint8_t portValue = 0;
  int sensorNr = 0;
  int nTimes = 1;

  cout << "Geef poort nummer (1-4): " << endl;
  cin >> portNr;
  cout << "Geef sensor type: " << endl;
  cout << "1: Color" << endl;
  cout << "2: Ultrasonic" << endl;
  cout << "3: Light" << endl;
  cout << "4: Touch" << endl;
  cin >> sensorNr;
  cout << "Hoe vaak checken: " << endl;
  cin >> nTimes;

  if(portNr == 1) portValue = 0x01;
  else if (portNr == 2) portValue = 0x02;
  else if (portNr == 3) portValue = 0x03;
  else if (portNr == 4) portValue = 0x04;
  else {
    cout << "Geen juiste keuze ontvangen.";
    return;
  }

  switch(sensorNr){
    case 1:
      for (int i = 0; i < nTimes; ++i) {
        if(BP.get_sensor(portValue,Color) == 0){
          cout << " red : " << Color.reflected_red;
          cout << " green : " << Color.reflected_green;
          cout << " blue : " << Color.reflected_blue;
          cout << " ambient : " << Color.ambient << endl;
          cout << " color code : " << (int) Color.color << endl;
        }
        sleep(5);
      }
      break;
    case 2:
      cout << "Case 2" << endl;
      for (int i = 0; i < nTimes; ++i) {
        cout << "In de case 2 loop nr : " << i << endl;
        if(BP.get_sensor(portValue,Ultrasonic) == 0){
          cout << Ultrasonic.cm << " cm" << endl;
        }
        sleep(5);
      }
      break;
    case 3:
      for (int i = 0; i < nTimes; ++i) {
        if(BP.get_sensor(portValue,Light) == 0){
          cout << " ambient : " << Light.ambient << endl;
          cout << " reflected : " << Light.reflected << endl;
        }
        sleep(5);
      }
      break;
    case 4:
      for (int i = 0; i < nTimes; ++i) {
        if(BP.get_sensor(portValue,Touch) == 0){
          cout << "pressed : " << Touch.pressed << endl;
        }
        sleep(5);
      }
      break;
  }

}


// Signal handler that will be called when Ctrl+C is pressed to stopHead the program
void exit_signal_handler(int signo){
  if(signo == SIGINT){
    BP.reset_all();    // Reset everything so there are no run-away motors
    exit(-2);
  }
}

int main(){
//thread checkBattery (batteryLevel);
  BP.detect();
  BP.reset_all();
  sleep(3);
  setSensors();
  sleep(2);
  stopHead();

  signal(SIGINT, exit_signal_handler);

  sensor_ultrasonic_t Ultrasonic;
  thread obstakelcheck (obstakelDetectie, Ultrasonic);

  while(true){
    sleep(1);
  }
  // bool loop = true;
  // while(loop){
  //   int keuze;
  //   cout << "Kies een funtie: "<< endl;
  //   cout << "1. Getsensor *werkt niet todat setSensors() werkt" << endl;
  //   cin >> keuze;

  //   switch(keuze){
  //     case 1:
  //       checkSensor();
  //       break;
  //     default:
  //       cout << "Het programma word afgebroken" << endl;
  //       loop = false;
  //       break;
  //   }
  // }
  return 0;
}

