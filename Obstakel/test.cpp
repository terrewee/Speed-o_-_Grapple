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

int measure(sensor_ultrasonic_t Ultrasonic){
    cout << "beep boop" << endl;
    int ding = BP.get_sensor(PORT_4, Ultrasonic);
    cout << ding << endl;
    if( ding == 0){
		cout << "Ultrasonic sensor (S4): "   << Ultrasonic.cm << "cm" << endl;
 	}
    sleep(5);
    return (int)Ultrasonic.cm;
}

void lookLeft(range obstakel, sensor_ultrasonic_t Ultrasonic){
    cout << "left" << endl;

    //spin left
    BP.set_motor_dps(PORT_B, -60);
    sleep(2);
    stop();

    int waarde = measure(Ultrasonic);
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

    int waarde = measure(Ultrasonic);
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

    float gemiddelde = measure(Ultrasonic);
    if (gemiddelde <= 10.0){
        obstakel.obstakelInRangeForward = true;
    }
    else{
        obstakel.obstakelInRangeForward = false;
    }
}

int main(){
    stop();
    //alles voor de sensor
    BP.detect();
    BP.reset_all();
    BP.set_sensor_type(PORT_4, SENSOR_TYPE_NXT_ULTRASONIC);
    sensor_ultrasonic_t Ultrasonic;


    range obstakel;
    sleep(5);
    lookForward(obstakel, Ultrasonic);
    lookLeft(obstakel, Ultrasonic);
    lookRight(obstakel, Ultrasonic);
    BP.reset_all();
}