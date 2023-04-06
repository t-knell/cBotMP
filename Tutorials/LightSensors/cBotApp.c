/*
 * cBotApp.c
 * Dieses Program veranschaulicht, wie man die Lichtsensorwerte
 * ausliest und auf dem Display ausgibt
 *
 */

#include <cBotApp.h>

#include <stdio.h>


void init(){

}

void loop(){
	//Sensordaten auslesen
	int sensor_left = getLightValue(SENSOR_LEFT);
	int sensor_right = getLightValue(SENSOR_RIGHT);


	//Auf Display ausgeben

	u8g2_ClearBuffer(display);

	int color_code = 1;
	u8g2_SetDrawColor(display, color_code);

	char text[20];
	u8g2_SetFont(display, u8g2_font_t0_22b_mr); // 2. Input ist die Schriftart und Größe

	int position_x = 0; //0 ist ganz links
	int position_y = (64 + u8g2_GetAscent(display))/2; // Y so, dass Text in der Mitte angezeigt wird

	sprintf(text, "%d | %d", sensor_left, sensor_right);
	u8g2_DrawStr(display, position_x, position_y, text);

	u8g2_SendBuffer(display);

}
