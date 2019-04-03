#include "BrickPi3.h"   // for BrickPi3
#include <iostream>     // for printf
#include <unistd.h>     // for usleep and sleep
#include <signal.h>     // for catching exit signals
#include <iomanip>		// for setw and setprecision

using namespace std;

sensor_ultrasonic_t = Ultrasonic1;

BrickPi3 BP;

void exit_signal_handler(int signo);

void exit_signal_handler(int signo)
{
if(signo == SIGINT)
    {
        BP.reset_all();
        exit(-2);
    }
}

void fwd(int lspd, int rspd)
{
    lspd *= -1;
    rspd *= -1;
    BP.set_motor_power(PORT_B, lspd);
    BP.set_motor_power(PORT_C, rspd);
}

void gradualSpeedIncrease()
{
    int currentSpeed = 0;
    if(BP.get_sensor(PORT_1, Ultrasonic1) == 0)
    {
        while(true)
        {
            if(Ultrasonic1.cm <= 20)
            {
                fwd(0,0);
                break;
            }
            currentSpeed += 5;
            fwd(currentSpeed, currentSpeed);
            cout << "Current speed: " << currentSpeed << endl;
            sleep(5);
        }
    }
    else
    {
        cout << "Error - no error.";
    }
}

int main()
{
    signal(SIGINT, exit_signal_handler);
    BP.detect();
}

