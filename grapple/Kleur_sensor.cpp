#include "BrickPi3.h" // for BrickPi3
#include <stdio.h>      // for printf
#include <unistd.h>     // for usleep
#include <signal.h>     // for catching exit signals
#include <iostream>
#include <string>


using namespace std;

BrickPi3 BP;


void setSensors(){
    BP.set_sensor_type(PORT_1,SENSOR_TYPE_NXT_ULTRASONIC);
    BP.set_sensor_type(PORT_2,SENSOR_TYPE_NXT_COLOR_FULL);
    BP.set_sensor_type(PORT_3,SENSOR_TYPE_NXT_LIGHT_ON);
    BP.set_sensor_type(PORT_4,SENSOR_TYPE_NXT_COLOR_FULL);
}

sensor_color_t      Color1;



int WhatIsInAColor (sensor_color_t Color1){
int_fast64_t color;
int colorchoice;
cout << "Welke kleur heeft het object" << endl;
cout << "1 : Rood " << endl;
cout << "2 : Blauw " << endl;
cout << "3 : Groen " << endl;
cout << "4 : Zwart " << endl;
cin >> color;
cout << color;
if (color == 1)     { colorchoice = Color1.reflected_red;}
if (color == 2)     { colorchoice = Color1.reflected_blue;}
if (color == 3)     { colorchoice = Color1.reflected_green;} 
if (color == 4)     { colorchoice = Color1.ambient;}
return colorchoice;
}

void FarbeNichtRichtig() {
    string answer;
    cout << "Dit object heeft niet de door u opgegeven kleur. Wilt u het object alsnog oppakken?" << endl;
    cout << "Ja of Nee" << endl;
    cin >> answer;
    if (answer == "Ja" )        {cout << "hij rijd naar achteren; op pak functie;"<< endl;}
    if (answer == "nee")        { cout << "Scout gaat weer zoeken"<< endl;} 
    
}

// int nepwaarde (){
//     int gemeten;
//     int gemetenwaarde = 0;
//     cout << "Welke kleur heeft het object sensor" << endl;
//     cout << "1: Rood " << endl;
//     cout << "2: Blauw " << endl;
//     cout << "3: Groen " << endl;
//     cout << "4: Zwart " << endl;
//     cin >> gemeten;
//     cout << gemeten;
//     if (gemeten == 1)     { gemetenwaarde = 1;}
//     if (gemeten == 2)     { gemetenwaarde  = 2;}
//     if (gemeten == 3)     { gemetenwaarde  = 3;} 
//     if (gemeten == 4)     { gemetenwaarde = 4;}
//     return gemetenwaarde;
     
}


char color_object (BP.get_sensor(PORT_1, Color1, int colorchoice){
    if (BP.get_sensor(PORT_1, Color1) == colorchoice)       { cout << "hij rijd naar achteren; op pak functie;" << endl;}
    if (BP.get_sensor(PORT_1, Color1) != colorchoice)       { FarbeNichtRichtig();}
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

  thread checkBattery (batteryLevel);

  while(true){
    sleep(5);
  }

  BP.reset_all();
  return 0;
}
// Signal handler that will be called when Ctrl+C is pressed to stop the program
void exit_signal_handler(int signo){
  if(signo == SIGINT){
    BP.reset_all();    // Reset everything so there are no run-away motors
    exit(-2);
  }
}


color_object (WhatIsInAColor());
}
