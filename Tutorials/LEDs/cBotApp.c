/*
 * cBotApp.c
 * Dieses Program veranschaulicht, wie man die LED Baender ansteuert
 *
 */

#include <cBotApp.h>

#include <stdio.h>
#include <unistd.h>


void init(){
	// LEDs funktionieren nach HSV (hue saturation value) Farbschema
	// https://web.cs.uni-paderborn.de/cgvb/colormaster/web/color-systems/hsv.html
	clearLeds();

	int sat = 100; // Saturation ist die Farbintensität -> 0 weiß -  100 volle Farbe
	int value = 10; // Helligkeit 0-100

	// Farben gehen von 0 bis 1535
	int red = 0;
	int green = 512;
	int blue = 1024;
	int white = getColorHSV(0, 0, 10); //Alles ist weiß, was Saturation = 0 hat

	// getColorHSV wandelt die Farben in etwas um, was die LEDs verstehen
	setLed(2, getColorHSV(red, sat, value));
	setLed(3, getColorHSV(blue, sat, value));
	setLed(4, getColorHSV(green, sat, value));
	setLed(5, white);

	updateLeds(); // LED Änderungen zu Band schicken

}


void loop(){



}
