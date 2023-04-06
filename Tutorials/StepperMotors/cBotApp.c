/*
 * cBotApp.c
 * Dieses Program veranschaulicht, wie man die Steppermotoren
 * ansteuert
 *
 */

#include <cBotApp.h>

#include <stdio.h>
#include <unistd.h>


void init(){

}

int drive = 0;

void loop(){
	//Fahren oder stoppen durch Knopfdruck
	float rpmLeft, rpmRight;
	rpmLeft = 15;
	rpmRight = 15;
	if(isPressed(BUTTON_RIGHT)){
		drive = !drive;
		while ( isPressed(BUTTON_RIGHT) ); // warten bis Button nichtmehr gedrückt
	}
	if(drive==1){
		setMotorRpm(rpmLeft, rpmRight);
	}else{
		setMotorRpm(0, 0);
	}
}
