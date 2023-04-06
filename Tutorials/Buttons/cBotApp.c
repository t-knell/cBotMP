/*
 * cBotApp.c
 * Dieses Programm veranschaulicht, wie man die Buttons ansteuert
 *
 */

#include <cBotApp.h>

#include <stdio.h>
#include <unistd.h>
#include "marioSong.h"

void displayPrint(char text[]){
	u8g2_ClearBuffer(display);

	int color_code = 1;
	u8g2_SetDrawColor(display, color_code);

	u8g2_SetFont(display, u8g2_font_t0_22b_mr); // 2. Input ist die Schriftart und Größe

	int position_x = 0; //0 ist ganz links
	int position_y = 50; // 0 ist ganz oben
	u8g2_DrawStr(display, position_x, position_y, text);

	u8g2_SendBuffer(display);
}

void init(){


}


void loop(){

	if(isPressed(BUTTON_RIGHT)){
			char text[] = "Right Button";
			displayPrint(text);
			while ( isPressed(BUTTON_RIGHT) ); // warten bis Button nichtmehr gedrückt
		}
	if(isPressed(BUTTON_LEFT)){
				char text[] = "Left Button";
				displayPrint(text);
				while ( isPressed(BUTTON_RIGHT) ); // warten bis Button nichtmehr gedrückt
			}
	if(isPressed(BUTTON_UP)){
				char text[] = "Up Button";
				displayPrint(text);
				while ( isPressed(BUTTON_RIGHT) ); // warten bis Button nichtmehr gedrückt
			}
	if(isPressed(BUTTON_DOWN)){
				char text[] = "Down Button";
				displayPrint(text);
				while ( isPressed(BUTTON_RIGHT) ); // warten bis Button nichtmehr gedrückt
			}

}
