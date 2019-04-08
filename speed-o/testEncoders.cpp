#include "BrickPi3.h"	// for BrickPi3
#include <iostream>	// for printf
#include <unistd.h>	// for usleep and sleep
#include <signal.h>	// for catching exit signals
#include <iomanip>	// for setw and setprecision

using namespace std;
BrickPi3 BP;

void exit_signal_handler(int signo);

void encodeMotor(int32_t pos)
{
	BP.set_motor_position_relative(PORT_A, pos);
}

void beweegKlauw()
{
	BP.offset_motor_encoder(PORT_A, BP.get_motor_encoder(PORT_A));
	BP.set_motor_limits(PORT_A, 50, 0);
	int32_t encoder = 0;
	while(true)
	{
		int rotatie = 0;
		cout << "Geef rotatie: ";
		cin >> rotatie;
		encodeMotor(rotatie);
		sleep(2);
		encoder = BP.get_motor_encoder(PORT_A);
		cout << "Positie: " << encoder << endl;
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
	BP.offset_motor_encoder(PORT_A, BP.get_motor_encoder(PORT_A));
	BP.set_motor_limits(PORT_A, 30, 0);
//	while(true)
//	{
		beweegKlauw();
//	}
}