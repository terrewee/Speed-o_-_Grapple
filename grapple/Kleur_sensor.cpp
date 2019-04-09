#include "BrickPi3.h" // for BrickPi3
#include <stdio.h>      // for printf
#include <unistd.h>     // for usleep
#include <signal.h>     // for catching exit signals

using namespace std;

BP.set_sensor_type(PORT_1, SENSOR_TYPE_NXT_COLOR_FULL);
sensor_color_t      Color1;
BP.set_sensor_type(PORT_2, SENSOR_TYPE_NXT_COLOR_FULL);
sensor_color_t      Color2;


int WhatIsInAColor (sensor_color_t){
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
void color_object(sensor_color_t Color1,  int powerA, int power){
    if (sensor_color_t Color1 == colorchoice)       { "hij rijd naar achteren; op pak functie;" }
    if (sensor_color_t Color1 =! colorchoice)       { FarbeNichtRichtig}
}



// void measure(sensor_color_t Color1, sensor_color_t Color4, int powerA, int powerB, sensor_ultrasonic_t ultrasonic, int &ticker){
   
//     if((BP.get_sensor(PORT_1, Color1) == 0)&&(BP.get_sensor(PORT_4, Color4) == 0)){
//     cout << "Color1 " << (int) Color1.color << " Color4 " << (int) Color4.color << endl;
        if     (Color1.color == 1 && Color4.color == 6)                         {rightcorrectie(powerA, powerB);}        //rechts wit links zwart
//         else if(Color1.color == 2 && Color4.color == 6)                         {mediumRight(powerA, powerB);}
//         else if((Color1.color > 2 && Color1.color < 5) && Color4.color == 1)    {slightRight(powerA, powerB);}
        
//         else if(Color1.color == 6 && Color4.color == 1)                         {leftcorrectie(powerA, powerB);}         //rechts zwart links wit
//         else if(Color1.color == 6 && Color4.color == 2)                         {mediumLeft(powerA, powerB);}
//         else if(Color1.color == 6 && (Color4.color > 2 && Color4.color < 5))    {slightLeft(powerA, powerB);}
        
//         else if(Color1.color == 6 && Color4.color == 6)                         {fwd(powerA, powerB);}                   //rechts zwart 
//         else if(Color1.color == 1 && Color4.color == 1)                         {crossroad(powerA, powerB, Color1, Color4);}  
        
//         ticker++;
//         cout << "ticker: " << ticker << endl;
//     }
// }

// getline(cin, color);
// if (color == 1)     { colorchoice = Color1.reflected_red;}
// if (color == 2)     { colorchoice = Color1.reflected_blue;}
// if (color == 3)     { colorchoice = Color1.reflected_green;} 
// if (color == 4)     { colorchoice = Color1.ambient;}