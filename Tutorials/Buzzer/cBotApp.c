/*
 * cBotApp.c
 * Dieses Programm veranschaulicht, wie man den Buzzer ansteuert
 *
 */

#include <cBotApp.h>

#include <stdio.h>
#include <unistd.h>
#include "marioSong.h"


void init(){
	playNote(f_e5, d_o);
	playPause(d_o/20);
	playNote(f_e5, d_o);
	playPause(d_o);
	playNote(f_e5, d_o);
	playPause(d_o);
	playNote(f_c5, d_o);
	playNote(f_e5, d_q);
	playNote(f_g5, d_q);
	playPause(d_q);
	playNote(f_g4, d_q);
	playPause(d_q);


}

void loop(){
	// Aufpassen, dass nicht zuviele Notes in den Buzzer Buffer geladen werde
	// sonst überlädt es den Buzzer und es kommt nur ein konstanter Ton
	// Wenn man folgenden Code ausführt, kommt statt dem ersten Teil der Mario
	// Melodie nur ein konstanter Ton
//	playNote(f_e5, d_o);
//	playPause(d_o/20);
//	playNote(f_e5, d_o);
//	playPause(d_o);
//	playNote(f_e5, d_o);
//	playPause(d_o);
//	playNote(f_c5, d_o);
//	playNote(f_e5, d_q);
//	playNote(f_g5, d_q);
//	playPause(d_q);
//	playNote(f_g4, d_q);
//	playPause(d_q);
}
