#include "BrickPi3.h" // for BrickPi3
#include <stdio.h>      // for printf
#include <unistd.h>     // for usleep
#include <signal.h>     // for catching exit signals
#include <iostream>
#include <string>
#include <thread>

using namespace std;

BrickPi3 BP;


void setSensors(){
    BP.set_sensor_type(PORT_1,SENSOR_TYPE_NXT_ULTRASONIC);
    BP.set_sensor_type(PORT_2,SENSOR_TYPE_NXT_COLOR_FULL);
    BP.set_sensor_type(PORT_3,SENSOR_TYPE_NXT_LIGHT_ON);
    BP.set_sensor_type(PORT_4,SENSOR_TYPE_NXT_COLOR_FULL);
}

sensor_color_t      Color1;



int WhatIsInAColor (){
int_fast64_t color;
int colorchoice = -1;
cout << "Welke kleur heeft het object" << endl;
cout << "1 : Rood " << endl;
cout << "2 : Blauw " << endl;
cout << "3 : Groen " << endl;
cout << "4 : Zwart " << endl;
cout << "5 : Wit " << endl;
cin >> color;
cout << color;
if (color == 1)          { colorchoice = 5;}
else if (color == 2)     { colorchoice = 2;}
else if (color == 3)     { colorchoice = 3;} 
else if (color == 4)     { colorchoice = 1;}
else if (color == 5)     { colorchoice = 6;}
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
     
//}


void color_object (int colorchoice){
  if (colorchoice == -1) {
    cout << "ey" << endl;
    return;
  }
  
  if (BP.get_sensor(PORT_2, ::Color1) == 0)
    cout << ::Color1.color << " sensor ," << colorchoice << " color choice"<< endl;
    if (::Color1.color == colorchoice)       { cout << "hij rijd naar achteren; op pak functie;" << endl;}
    else if ( ::Color1.color != colorchoice)       { FarbeNichtRichtig();}
  else { cout << BP.get_sensor(PORT_2, ::Color1); }
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
  color_object (WhatIsInAColor());


  while(true){
    sleep(5);
  }

  BP.reset_all();
  return 0;
  
  

}
