#include "BrickPi3.h" // for BrickPi3
#include <stdio.h>      // for printf
#include <unistd.h>     // for usleep
#include <signal.h>     // for catching exit signals
#include <iostream>     //voor cout enz.

BrickPi3 BP;
using namespace std;

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
  bool obstakelcurrent= false;
};;

void stop(){
  //reset de motor dps
  BP.set_motor_dps(PORT_A, 0);
}
void lookLeft(range obstakel){
  //look left
  cout << "left" << endl;
  BP.detect();
  BP.set_sensor_type(PORT_2, SENSOR_TYPE_NXT_ULTRASONIC);
  sensor_ultrasonic_t Ultrasonic;
  BP.set_motor_dps(PORT_A, -60);
  sleep(2);
  stop();
  
  int som = 0;
  for(unsigned int i = 0; i < 3; i++){
    BP.get_sensor(PORT_2, Ultrasonic);
    cout << "Afstand" << (int) Ultrasonic.cm << endl;
    som += (int) Ultrasonic.cm;
    sleep(2);
    
  }
  float gemiddelde = som/3;
  if (gemiddelde <= 10.0){
    obstakel.obstakelInRangeLeft = true;
  }
  else{
    obstakel.obstakelInRangeLeft = false;
  }
  //reset to middle
  BP.set_motor_dps(PORT_A, 60);
  sleep(2);
  stop();
}

void lookRight(range obstakel){
  //look right
  cout << "right" << endl;
  BP.detect();
  BP.set_sensor_type(PORT_2, SENSOR_TYPE_NXT_ULTRASONIC);
  sensor_ultrasonic_t Ultrasonic;
  BP.set_motor_dps(PORT_A, 60);
  sleep(2);
  stop();
  
  int som = 0;
  for(unsigned int i = 0; i < 3; i++){
    BP.get_sensor(PORT_2, Ultrasonic);
    cout << "Afstand" << (int) Ultrasonic.cm << endl;
    som += (int) Ultrasonic.cm;
    sleep(2);
    
  }
  float gemiddelde = som/3;
  if (gemiddelde <= 10.0){
    obstakel.obstakelInRangeLeft = true;
  }
  else{
    obstakel.obstakelInRangeLeft = false;
  }
  //reset to middle
  BP.set_motor_dps(PORT_A, -60);
  sleep(2);
  stop();
}

void lookForward(range obstakel){
  cout << "test2" << endl;
  BP.detect();
  BP.set_sensor_type(PORT_2, SENSOR_TYPE_NXT_ULTRASONIC);
  sensor_ultrasonic_t Ultrasonic;
  int som = 0;
  for(unsigned int i = 0; i < 3; i++){
    cout << "test3" << endl;
    if(BP.get_sensor(PORT_2, Ultrasonic) != 0){
      cout << "Afstand" << (int) Ultrasonic.cm << endl;
      som += (int) Ultrasonic.cm;
      sleep(2);
    }
  }
  float gemiddelde = som/3;
  if (gemiddelde <= 10.0){
    obstakel.obstakelInRangeForward = true;
  }
  else{
    obstakel.obstakelInRangeForward = false;
  }
}

void obstakelDetectie(range obstacle){
  //main van obstakel
    BP.detect();
    BP.set_sensor_type(PORT_2, SENSOR_TYPE_NXT_ULTRASONIC);
    sensor_ultrasonic_t Ultrasonic;
    //BP.set_motor_power(PORT_A, 5);

    cout << "Forward" << endl;
    lookForward(obstacle);

    cout << "left" << endl;
    lookLeft(obstacle);

    cout << "right" << endl;
    lookRight(obstacle);
    // lookForward(Ultrasonic, obstacle);
    // stop();
    // while (obstacle.obstakelInRangeForward== true){
    //     stop();
    //     lookLeft(Ultrasonic, obstacle);
    //     if (obstacle.obstakelInRangeLeft == true){
    //         cout << "cant go left" << endl;
    //     }
    //     else{
    //         cout << "can go left" << endl;
    //     }
    //     sleep (1);
    //     lookRight(Ultrasonic, obstacle);
    //     if (obstacle.obstakelInRangeRight == true){
    //         cout << "cant go right" << endl;
    //     }
    //     else{
    //         cout << "can go right" << endl;
    //     }
    //     sleep (1);

    // }
}

/*
  Author:       Duur
  Description:  setSensors set all the sensors for a specific robot
                and immediatly sets them.
*/
void setSensors(){
  //BP.set_sensor_type();
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

int main(){
//thread checkBattery (batteryLevel);
  //setSensors();
  range obstacle;
  bool loop = true;
  while(loop){
    int keuze;
    cout << "Kies een funtie: "<< endl;
    cout << "1. Getsensor *werkt niet todat setSensors() werkt" << endl;
    cout << "2. Obstakel detectie" << endl;
    cin >> keuze;

    switch(keuze){
      case 1:
        checkSensor();
        break;
      case 2:
        obstakelDetectie(obstacle);
        break;
      default:
        cout << "Het programma word afgebroken" << endl;
        loop = false;
        break;
    }
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