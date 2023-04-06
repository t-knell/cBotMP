/*
 * controlDisplay.c
 * Dieses Program veranschaulicht, wie man den Display ansteuern kann
 */

#include <cBotApp.h>

#include <stdio.h>


void init(){

}

void loop(){
	// init display buffer
	u8g2_ClearBuffer(display);

	int color_code = 1;
	u8g2_SetDrawColor(display, color_code);

	char text[] = "Hello World!";
	u8g2_SetFont(display, u8g2_font_t0_22b_mr); // 2. Input ist die Schriftart und Größe

	int position_x = 0; //0 ist ganz links
	int position_y = 50; // 0 ist ganz oben
	u8g2_DrawStr(display, position_x, position_y, text);

	u8g2_SendBuffer(display);

}
