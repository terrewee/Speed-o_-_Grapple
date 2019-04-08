#include "BrickPi3.h"	// for BrickPi3
#include <iostream>	// for printf
#include <unistd.h>	// for usleep and sleep
#include <signal.h>	// for catching exit signals
#include <iomanip>	// for setw and setprecision

using namespace std;
BrickPi3 BP;

void exit_signal_handler(int signo);

void fwd(int lspd , int rspd)
{
	BP.set_motor_power(PORT_B, -lspd);
	BP.set_motor_power(PORT_C, -rspd);
}

void encodeMotor(int32_t pos)
{
	BP.set_motor_position_relative(PORT_A, pos);
}

void meetBeweging()
{
	BP.set_motor_limits(PORT_A, 100, 0);
	BP.offset_motor_encoder(PORT_A, BP.get_motor_encoder(PORT_A));
	int32_t encoder = 0;
signal(SIGINT, exit_signal_handler); // register the exit function for Ctrl+C
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

void beweegKlauw()
{
	BP.set_motor_limits(PORT_A, 100, 0);
	BP.offset_motor_encoder(PORT_A, BP.get_motor_encoder(PORT_A));
	int32_t encoder = 0;
	encodeMotor(-50);
	while(encoder > -110)
	{
		encodeMotor(-5);
		usleep(250000);	// wellicht kleiner maken
		encoder = BP.get_motor_encoder(PORT_A);
		cout << "Positie: " << encoder << endl;
	}
	// sluit klauw
	cout << "\n\n\n";
	while(encoder < 0)
	{
		encodeMotor(5);
		usleep(250000);	// kleiner maken
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
	signal(SIGINT, exit_signal_handler);
	BP.detect();
	fwd(100, 100);
	//beweegKlauw();
}
