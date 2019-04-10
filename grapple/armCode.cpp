
#include "grappleHead.h"  // our main grapple headder

using namespace std;


void encodeMotor(int32_t pos){
	BP.set_motor_position_relative(PORT_A, pos);
}

void brengNaarKantelPunt(){
	BP.set_motor_limits(PORT_A, 40, 0);
	encodeMotor(-50);
}

void gelijdelijkDownLoop(){
	int32_t encoder = -50;
	while(encoder > -110){
		encodeMotor(-5);
		usleep(500000);
		encoder = encoder - 5;
	}
}

void klauwOmhoog(){
	BP.set_motor_limits(PORT_A, 50, 0);
	encodeMotor(130);	// zelfde als totale neerwaartse beweging
}

void klauwOpen(){
	BP.set_motor_limits(PORT_D, 60, 0);
	BP.set_motor_position_relative(PORT_D, -180);
}
void klauwDicht(){
	BP.set_motor_limits(PORT_D, 60, 0);
	BP.set_motor_position_relative(PORT_D, 180);
}