/*
 * cBotApp.c
 * Dieses Program veranschaulicht, wie man Werte von den Ultraschallsensoren
 * ausliest und auf dem Bildschirm ausgibt
 * */

#include <cBotApp.h>

#include <stdio.h>


void init(){

}

void loop(){
	//Sensordaten auslesen
	int sensor_links = getRangeMm(SENSOR_LEFT);
	int sensor_mitte = getRangeMm(SENSOR_MIDDLE);
	int sensor_rechts = getRangeMm(SENSOR_RIGHT);


	//Auf Display ausgeben

	// init display buffer
	u8g2_ClearBuffer(display);

	int color_code = 1;
	u8g2_SetDrawColor(display, color_code);

	char text[20];
	u8g2_SetFont(display, u8g2_font_t0_22b_mr); // 2. Input ist die Schriftart und Größe

	int position_x = 0; //0 ist ganz links
	int position_y = u8g2_GetAscent(display); // Y so, dass Text ganz oben angezeigt wird

	sprintf(text,"L: %dmm", sensor_links);
	u8g2_DrawStr(display, position_x, position_y, text);

	position_y = (64 + u8g2_GetAscent(display)) / 2; //Y in der Mitte
	sprintf(text,"M: %dmm", sensor_mitte);
	u8g2_DrawStr(display, position_x, position_y, text);

	position_y = 63; //Y unten
	sprintf(text,"R: %dmm", sensor_rechts);
	u8g2_DrawStr(display, position_x, position_y, text);

	u8g2_SendBuffer(display);

}
