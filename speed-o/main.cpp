#include "BrickPi3.h"   // for BrickPi3                                                     //TE DOEN
#include <iostream>     // for printf
#include <unistd.h>     // for usleep and sleep
#include <signal.h>     // for catching exit signals
#include <iomanip>		// for setw and setprecision

using namespace std;

sensor_color_t Color1;                                                                      //check de ports voor cohesie in sensornamen
sensor_ultrasonic_t Ultrasonic2;
sensor_touch_t Touch3;
sensor_light_t Light4;

BrickPi3 BP;

void exit_signal_handler(int signo);

void setSensors()                                                                           //check of de ports juist ingesteld staan
{
	BP.set_sensor_type(PORT_1,SENSOR_TYPE_NXT_COLOR_FULL);
	BP.set_sensor_type(PORT_2,SENSOR_TYPE_NXT_ULTRASONIC);
	BP.set_sensor_type(PORT_3,SENSOR_TYPE_NXT_LIGHT_ON);
	BP.set_sensor_type(PORT_4,SENSOR_TYPE_NXT_COLOR_FULL);
}

void fwd(int lspd , int rspd)
{
	BP.set_motor_power(PORT_B, lspd);
	BP.set_motor_power(PORT_C, rspd);
}

/*
    Author: Joram van Leeuwen
    Description: Functie voor het detecteren van een object binnen een gegeven afstand en met een als parameter gegeven kleurcode
                 vervolgens rijdt de robot tot de klauw boven het object hangt
*/

void detecteerObject(int kleurVanObject)
{
    if(Ultrasonic2.cm <= 10 && Color1.color == kleurVanObject) //10cm als voorbeeld -- kan worden aangepast
    {}                                                                                      //rijd totdat het object onder de klauw ligt -- encoder?
}

/*
    Author: Joram van Leeuwen
    Description: Grijpt een object in de klauw indien de touch sensor iets aanraakt. Dit is een puur iets waarvan ik niet zeker ben of het werkt
*/
void grijpObject(int motorKracht)
{
    if(Touch3.pressed == true)                                                              //controleer of touch werkt
    {
        BP.set_motor_power(PORT_, motorKracht)                                              //set motorport voor sluiten grijper
        sleep(2);
        BP.set_motor_power(PORT_, motorKracht)                                              //set motorport voor omhoog halen grijper
    }
}

void exit_signal_handler(int signo)
{
if(signo == SIGINT)
    {
        BP.reset_all();
        exit(-2);
    }
}

int main()
{
    signal(SIGINT, exit_signal_handler);
  	BP.detect();
	setSensors();

    while(true)
    {
        detecteerObject();                                                                  //set kleurcode
        sleep(3);   //sleep om te voorkomen dat het té abrupt gaat
        grijpObject();                                                                      //set motorkracht
    }
}
