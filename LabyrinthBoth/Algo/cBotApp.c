/*
 * Algo
 * Dieses Programm ist ein erster Prototyp für die Navigation durch das Labyrinth
 * mit Hilfe des Trémaux Algorithmus.
 * Der Roboter fährt nicht selbst, sondern muss immer in das nächste Feld gesetzt werden. sondern analysiert nach Knopfdruck nur das Feld
 * Roboter analysier Feld in dem er gerade steht und gibt dann auf dem Bildschirm aus, wohin er als nächstes fahren möchte.
 * Wäre auch cool, wenn er bei der Analyse immer angeben würde, was er gerade um sich herum "sieht" -> NSEW Counter ausgeben
 * Könnte auch ENUM erstellen fuer Nord Sued Ost West
 */


#include "cBotApp.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

typedef struct {
	// Braucht jetzt 4 Byte, könnte man theoretisch auf 2 Byte reduzieren, aber super viel Aufwand und schlecht übersichtlich
	// Wand = 9, nicht durchfahren & offen = 0, 1 mal durchfahren = 1, 2 mal durchfahren = 2
	char counter_n;
	char counter_s;
	char counter_e;
	char counter_w;
} cell;

const cell base_cell = {0, 0, 0, 0};
cell map[15][15];
int pos[2] = {7, 7}; // Momentane Position im Labyrinth - ROW - COL
int orientation = 2; // N=0 S=1 E=2 W=3
int field_counter = 0; // Anzahl der zurückgelegten Felder
int move_to = 0; // Richtungsanweisung für die nächste Bewegung


void init(){
	// LEDs setzen
	clearLeds();
	int bright_white = getColorHSV(0, 0, 50);
	int dim_white = getColorHSV(0,0,1);
	for (int i=0; i<8; i++){
		setLed(i, dim_white);
	}
	setLed(8, bright_white);
	setLed(9, bright_white);
	updateLeds();

	for (int i; i<15; i++){
		for (int j; j<15; j++){
			map[i][j] = base_cell;
		}
	}

}

// Überprüft, ob das Ziel erreicht wurde
char goalReached(){
	int counter = 0;
	int sleep_duration = 20;
	int value_threshold = 200;
	int time_threshold = 1000; //in ms
	int sensor_left = getLightValue(SENSOR_LEFT);
	int sensor_right = getLightValue(SENSOR_RIGHT);
	while (1){
		if (sensor_left >= value_threshold && sensor_right >= value_threshold){
				return 0;
		}
		else{
			counter += sleep_duration;
		}
		if (counter >= time_threshold){
			return 1;
		}
		HAL_Delay(sleep_duration);
	}
}

// Diese Funktion sucht nach Wänden um den Roboter und updated anschließend das Mapping
// ToDo: In zwei Funktionen splitten - Wände suchen und Mapping aktualisieren
// ToDo: Einen richtigen ErrorState herbeiführen können, wenn etwas falsch läuft -> Wie Py Exception
void checkForWalls(){
	cell *current_cell;
	current_cell = &map[pos[0]][pos[1]];
	int wall_threshold = 100; // mm
	int sensor_values[] = {getRangeMm(SENSOR_LEFT), getRangeMm(SENSOR_MIDDLE), getRangeMm(SENSOR_RIGHT)};
	int sensor_values_nsew[4];

	// Sensor Werte in ihre Himmelsrichtung Orientierung umtragen
	if (orientation==0){
		sensor_values_nsew[0] = sensor_values[1];
		sensor_values_nsew[1] = 999; // Von da sind wir gerade gekommen -> High value damit in der Abfrage danach es nicht reinrutscht
		sensor_values_nsew[2] = sensor_values[2];
		sensor_values_nsew[3] = sensor_values[0];
	} else if(orientation==1){
		sensor_values_nsew[0] = 999;
		sensor_values_nsew[1] = sensor_values[1];
		sensor_values_nsew[2] = sensor_values[0];
		sensor_values_nsew[3] = sensor_values[2];
	} else if(orientation==2){
		sensor_values_nsew[0] = sensor_values[0];
		sensor_values_nsew[1] = sensor_values[2];
		sensor_values_nsew[2] = sensor_values[1];
		sensor_values_nsew[3] = 999;
	} else if(orientation==3){
		sensor_values_nsew[0] = sensor_values[2];
		sensor_values_nsew[1] = sensor_values[0];
		sensor_values_nsew[2] = 999;
		sensor_values_nsew[3] = sensor_values[1];
	} else{
		// Something went wrong
	}

	// Checken ob in den Himmelsrichtungen eine Wand ist und falls ja, dies in der Karte vermerken
	for (int i=0; i<4; i++) {
		if (sensor_values_nsew[i] <= wall_threshold) {
			if (i == 0) {
				current_cell->counter_n = 9;
			} else if (i == 1) {
				current_cell->counter_s = 9;
			} else if (i == 2) {
				current_cell->counter_e	= 9;
			} else {
				current_cell->counter_w = 9;
			}
		}
	}
}

// Diese Funktion gibt das aktuelle Feld auf dem Display aus.
// Zeigt an, wo Wände sind und die Tremaux Counter
void displayCell(){
	cell *current_cell;
	current_cell = &map[pos[0]][pos[1]];


	char text[20];
	int color_code = 1;
	u8g2_ClearBuffer(display);
	u8g2_SetDrawColor(display, color_code);
	u8g2_SetFont(display, u8g2_font_6x10_tr);

	sprintf(text, "%d", current_cell->counter_n);
	u8g2_DrawStr(display, (128 - u8g2_GetStrWidth(display, text))/2, 10, text);

	sprintf(text, "%d", current_cell->counter_s);
	u8g2_DrawStr(display, (128 - u8g2_GetStrWidth(display, text))/2, 60, text);

	sprintf(text, "%d", current_cell->counter_e);
	u8g2_DrawStr(display, 128 - u8g2_GetStrWidth(display, text), 40, text);

	sprintf(text, "%d", current_cell->counter_w);
	u8g2_DrawStr(display, 0, 40, text);

	u8g2_SendBuffer(display);



}


void loop(){

	// Auf Knopfdruck warten
	if(isPressed(BUTTON_RIGHT)){
		// Check, ob am Ziel angekommen
		if(goalReached()){
			u8g2_ClearBuffer(display);

			int color_code = 1;
			u8g2_SetDrawColor(display, color_code);

			char text[] = "Ziel erreicht!";
			u8g2_SetFont(display, u8g2_font_6x10_tr); // 2. Input ist die Schriftart und Größe

			int position_x = 0; //0 ist ganz links
			int position_y = (64 + u8g2_GetAscent(display))/2; // Y so, dass Text in der Mitte angezeigt wird

			u8g2_DrawStr(display, position_x, position_y, text);

			u8g2_SendBuffer(display);
		}

		// Mit Ultraschall schauen, wo Wände sind und in Karte speichern
		checkForWalls();

		// Feld auf Display ausgeben
		displayCell();
		// Falls erste Bewegung, dann random ein Feld wählen
		if (field_counter == 0){


		}


		// Wenn nur der Weg offen ist (Counter <=1), durch den wir gerade gefahren sind, dann durch diesen fahren und den Counter aktualisieren
		// Wenn zwei Wege offen sind, dann den nehmen, durch den wir nicht gefahren sind -> Pfad geht weiter
		// Wenn mehr als zwei Wege offen sind und wir das erste Mal an dieser Kreuzung sind, dann einen zufälligen, nicht befahrenen Weg wählen & Counter aktualisieren
		// Wenn mehr als zwei Wege offen sind, aber andere Wege in der Kreuzung markiert sind und der eben durchfahrene Weg keinen Counter=2 Wert hat, dann wieder umkehren
		// Wenn mehr als zwei offen sind, die Kreuzung schon Markierungen hat und der eben durchfahrene Weg Counter=2 hat, dann den Weg mit den niedrigsten Markierungen wählen. Falls gleichstand, dann random einen wählen
		// Wenn das alles nicht der Fall ist, dann Selbstzerstörung

		// Warte, bis Button nicht mehr gedrückt ist
		while ( isPressed(BUTTON_RIGHT) );
		}



		// Entscheidung auf Bilschirm ausgeben und Aufforderung, ins nächste Feld platziert zu werden
		// Position aktualisieren
		// Orientierung aktualisieren. Orientierung == Richtung, in die Zuletzt gefahren wurde
		// Tremaux Counter aktualisieren
		// Anzahl der zurückgelegten Felder aktualisieren
}


