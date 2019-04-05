#include "BrickPi3.h" // for BrickPi3
#include <stdio.h>      // for printf
#include <unistd.h>     // for usleep
#include <signal.h>     // for catching exit signals
#include <iostream>     //voor cout enz.

BrickPi3 BP;
using namespace std;

struct range {
  bool obstakelInRangeLeft = false;
  bool obstakelInRangeRight = false;
  bool obstakelInRangeForward = false;
  bool obstakelcurrent= false;
};

void stop(){
    //reset de motor dps
    BP.set_motor_dps(PORT_B, 0);
}

int measure(sensor_ultrasonic_t Ultrasonic);

    if(BP.get_sensor(PORT_4, Ultrasonic2) == 0){
		cout << "Ultrasonic sensor (S4): "   << Ultrasonic.cm << "cm" << endl;
 	}
    return (int)Ultrasonic.cm
}

void lookLeft(range obstakel, sensor_ultrasonic_t Ultrasonic){
    cout << "left" << endl;

    //spin left
    BP.set_motor_dps(PORT_B, -60);
    sleep(2);
    stop();

    int waarde = check(Ultrasonic);
    if (waarde <= 10.0){
        obstakel.obstakelInRangeLeft = true;
    }
    else{
        obstakel.obstakelInRangeLeft = false;
    }

    //reset to middle
    BP.set_motor_dps(PORT_B, 60);
    sleep(2);
    stop();
}

void lookRight(range obstakel, sensor_ultrasonic_t Ultrasonic){
    cout << "right" << endl;

    //spin right.
    BP.set_motor_dps(PORT_B, 60);
    sleep(2);
    stop();

    int waarde = check(Ultrasonic);
    if (waarde <= 10.0){
        obstakel.obstakelInRangeLeft = true;
    }
    else{
        obstakel.obstakelInRangeLeft = false;
    }

    //reset to middle
    BP.set_motor_dps(PORT_B, -60);
    sleep(2);
    stop();
}

void lookForward(range obstakel,sensor_ultrasonic_t Ultrasonic){
    cout << "forward" << endl;

    float gemiddelde = check(Ultrasonic);
    if (gemiddelde <= 10.0){
        obstakel.obstakelInRangeForward = true;
    }
    else{
        obstakel.obstakelInRangeForward = false;
    }
}

int main(){
    //alles voor de sensor
    BP.detect();
    BP.set_sensor_type(PORT_4, SENSOR_TYPE_NXT_ULTRASONIC);
    sensor_ultrasonic_t Ultrasonic;


    range obstakel
    lookForward(obstakel, Ultrasonic);
    lookLeft(obstakel, Ultrasonic);
    lookRight(obstakel, Ultrasonic);
}