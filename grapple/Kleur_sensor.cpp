#include "BrickPi3.h" // for BrickPi3
#include <stdio.h>      // for printf
#include <unistd.h>     // for usleep
#include <signal.h>     // for catching exit signals


using namespace std;
// BrickPi3 BP;

// BP.set_sensor_type(PORT_1, SENSOR_TYPE_NXT_COLOR_FULL);
// sensor_color_t      Color1;
// BP.set_sensor_type(PORT_2, SENSOR_TYPE_NXT_COLOR_FULL);
// sensor_color_t      Color2;


int WhatIsInAColor (){
int color;
cout << "Welke kleur heeft het object" << endl;
cout << "1: Rood " << endl;
cout << "2: Blauw " << endl;
cout << "3: Groen " << endl;
cout << "4: Zwart " << endl;
getline(cin, color);
if (color == 1)     { colorchoice = 1;}
if (color == 2)     { colorchoice = 2;}
if (color == 3)     { colorchoice = 3;} 
if (color == 4)     { colorchoice = 4;}
return colorchoice;
}

int nepwaarde (){
    int gemeten;
    cout << "Welke kleur heeft het object sensor" << endl;
    cout << "1: Rood " << endl;
    cout << "2: Blauw " << endl;
    cout << "3: Groen " << endl;
    cout << "4: Zwart " << endl;
    getline(cin, gemeten);
    if (gemeten == 1)     { gemetenwaarde = 1;}
    if (gemeten == 2)     { gemetenwaarde  = 2;}
    if (gemeten == 3)     { gemetenwaarde  = 3;} 
    if (gemeten == 4)     { gemetenwaarde = 4;}
    return gemetenwaarde;
    
}

char FarbeNichtRichtig() {
    string answer
    cout << "Dit object heeft niet de door u opgegeven kleur. Wilt u het object alsnog oppakken?" << endl;
    cout << "Ja of Nee"
    getline( cin, answer)
    if (answer == "Ja" )        {cout << "hij rijd naar achteren; op pak functie;"<< endl;}
    if (answer == "nee")        { cout << "Penis"<< endl;} 

}
void color_object(nepwaarde,  WhatIsInAColor, int powerA, int power){
    if (gemetenwaarde == colorchoice)       { cout << "hij rijd naar achteren; op pak functie;" << endl;}
    if (gemetenwaarde != colorchoice)       { FarbeNichtRichtig()}
}

int main(){
//       signal(SIGINT, exit_signal_handler); // register the exit function for Ctrl+C
//   BP.detect();
//   BP.reset_all();
//   for (int i = 0; i < 5; ++i){
//     cout << ".";
//     if (i == 3){
//       setSensors();
//     }
//     sleep(1);
//   }
//   cout << endl << "Initialized" << endl;

//   thread checkBattery (batteryLevel);

//   while(true){
//     sleep(5);
//   }

//   BP.reset_all();
//   return 0;
// }
// // Signal handler that will be called when Ctrl+C is pressed to stop the program
// void exit_signal_handler(int signo){
//   if(signo == SIGINT){
//     BP.reset_all();    // Reset everything so there are no run-away motors
//     exit(-2);
//   }
// }

WhatIsInAColor(sensor_color_t)
}
