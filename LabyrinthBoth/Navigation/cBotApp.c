/*
 * Navigation
 * Versuch vollständige Navigation mit Algo etc in das Labyrinth zu integrieren
 */


#include "cBotApp.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

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
int current_pos[2] = {7, 7}; // Momentane Position im Labyrinth - ROW - COL
int next_pos[2] = {7, 7};
cell *current_cell;
cell *next_cell;
char orientation = 0; // N=0 S=1 E=2 W=3
int movement_counter = 0; // Anzahl der zurückgelegten Felder
int next_direction = 0; // Richtungsanweisung für die nächste Bewegung

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
	int wall_threshold = 200; // mm
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

// Wähle die Richtung mit dem niedrigsten erlaubten Counter Wert
int lowestCounterDirection(){
	for (int i = 0; i<2; i++) {
		if (current_cell->counter_n == i) {
			return 0;
		}
		if (current_cell->counter_s == i) {
			return 1;
		}
		if (current_cell->counter_e == i) {
			return 2;
		}
		if (current_cell->counter_w == i) {
			return 3;
		}
	}
	return 999;
}

// Zeige die Richtung auf dem Display an, in die der Roboter als nächstes fahren möchte
void displayMovementDecision(){
	char direction[10];
	if (next_direction == 0) {
		strcpy(direction, "North");
	}
	if (next_direction == 1) {
		strcpy(direction, "South");
	}
	if (next_direction == 2) {
		strcpy(direction, "East");
	}
	if (next_direction == 3) {
		strcpy(direction, "West");
	}
	u8g2_ClearBuffer(display);

	int color_code = 1;
	u8g2_SetDrawColor(display, color_code);

	char text[50] = "Move towards: ";
	strcat(text, direction);
	u8g2_SetFont(display, u8g2_font_6x10_tr); // 2. Input ist die Schriftart und Größe

	int position_x = 0; //0 ist ganz links
	int position_y = (64 + u8g2_GetAscent(display))/2; // Y so, dass Text in der Mitte angezeigt wird

	u8g2_DrawStr(display, position_x, position_y, text);

	u8g2_SendBuffer(display);
}

// Gib die Anzahl der offenen Wege zurück
char getNumberOfExits() {
	char counter;
	if (current_cell->counter_n < 2) {
		counter += 1;
	}
	if (current_cell->counter_s < 2) {
		counter += 1;
	}
	if (current_cell->counter_e < 2) {
		counter += 1;
	}
	if (current_cell->counter_w < 2) {
		counter += 1;
	}
	return 99;
}

// Gibt die entgegengestzte Himmelsrichtung zurück
char getOppositeDirection(char direction) {
	if (direction == 0){
		return 1;
	}
	if (direction == 1){
		return 0;
	}
	if (direction == 2){
		return 3;
	}
	if (direction == 3){
		return 2;
	}
	return 99;
}

// Gibt zurück, ob der Roboter schonmal in der Zelle war
char cellVisited() {
	char counter;
	for (int i = 1; i < 3; i++){
		if (current_cell->counter_n == i) {
			counter += i;
		}
		if (current_cell->counter_s == i) {
			counter += i;
		}
		if (current_cell->counter_e == i) {
			counter += i;
		}
		if (current_cell->counter_w == i) {
			counter += i;
		}
		if (counter > 1){
			return 1;
		}
	}
	return 0;
}

// Gibt Tremaux Counter basierend auf der Himmelsrichtung zurück
char getCounterFromDirection(char direction) {
	if (direction == 0){
		return current_cell->counter_n;
	}
	if (direction == 1){
		return current_cell->counter_s;
	}
	if (direction == 2){
		return current_cell->counter_e;
	}
	if (direction == 3){
		return current_cell->counter_w;
	}
	return 99;
}

// Gibt zurück, in welche Richtung laut dem Tremaux Algorithmus als nächstes gefahren werden soll
char getTremauxDirection() {
	char exits = getNumberOfExits();
	char opposite_direction = getOppositeDirection(orientation);
	char lowest_counter_direction = lowestCounterDirection();
	// Wenn nur der Weg offen ist (Counter ==1), durch den wir gerade gefahren sind, dann durch diesen fahren und den Counter aktualisieren
	if (exits == 1){
		return opposite_direction;
	}
	if (exits == 2) {
		return lowest_counter_direction;
	}
	if (!cellVisited()) {
		return lowest_counter_direction;
	}
	char opposite_direction_counter = getCounterFromDirection(opposite_direction);
	if ( opposite_direction_counter < 2) {
		return opposite_direction;
	}
	return lowest_counter_direction;
}

// Aktualisiert die Tremaux Counter
void updateTremauxCounters() {
	if (orientation == 0){
		current_cell->counter_n += 1;
		next_cell->counter_s += 1;
	}
	if (orientation == 1){
		current_cell->counter_s += 1;
		next_cell->counter_n += 1;
	}
	if (orientation == 2){
		current_cell->counter_e += 1;
		next_cell->counter_w += 1;
	}
	if (orientation == 3){
		current_cell->counter_w += 1;
		next_cell->counter_e += 1;
	}
}

// Berechnet die nächste Position und speichert sie ab
void updateNextPosition() {
	if (next_direction == 0) {
		next_pos[0] -= 1;
	} else if (next_direction == 1) {
		next_pos[0] += 1;
	} else if (next_direction == 2) {
		next_pos[1] += 1;
	} else if (next_direction == 3) {
		next_pos[1] -= 1;
	}
}

// Um 90° drehen
void turn90(char direction) {
	// 0 ist rechts
	int rpm_left, rpm_right;
	if (direction == 0) {
		rpm_left = 15;
		rpm_right = -15;
	} else if (direction == 1) {
		rpm_left = -15;
		rpm_right = 15;
	}
	setMotorRpm(rpm_left, rpm_right);
	HAL_Delay(2400);
	rpm_left = 0;
	rpm_right = 0;
	setMotorRpm(rpm_left, rpm_right);
}

// Um 180° drehen
void turn180() {
	int rpm_left = -15, rpm_right = 15;
	setMotorRpm(rpm_left, rpm_right);
	HAL_Delay(470

			0);
	rpm_left = 0;
	rpm_right = 0;
	setMotorRpm(rpm_left, rpm_right);
}

// Ein Feld vorwärts fahren
void moveOneFieldForward() {
	int speed = 15;
	setMotorRpm(speed, speed);
	HAL_Delay(8120);
	speed = 0;
	setMotorRpm(speed, speed);
}

// Orientierung zur gegebenen Richtung ändern
void turnToDirection(char direction) {
	if (orientation == 0) {
		if (direction == 0) {
			return;
		}
		if (direction == 1) {
			turn180();
		} else if (direction == 3) {
			turn90(1);
		} else if (direction == 2) {
			turn90(0);
		}
	} else if (orientation == 1) {
		if (direction == 1) {
			return;
		}
		if (direction == 0) {
			turn180();
		} else if (direction == 2) {
			turn90(1);
		} else if (direction == 3) {
			turn90(0);
		}
	} else if (orientation == 2) {
		if (direction == 2) {
			return;
		}
		if (direction == 3) {
			turn180();
		} else if (direction == 0) {
			turn90(1);
		} else if (direction == 1) {
			turn90(0);
		}
	} else if (orientation == 3) {
		if (direction == 3) {
			return;
		}
		if (direction == 2) {
			turn180();
		} else if (direction == 1) {
			turn90(1);
		} else if (direction == 0) {
			turn90(0);
		}
	}
}


// Orientiere dich in die richtige Richtung und fahr ins nächste Feld
void moveToNextField(char direction) {
	if (direction == orientation) {
		moveOneFieldForward();
	} else {
		turnToDirection(direction);
		moveOneFieldForward();
	}
}


void init(){
	// Welcome Bildschirm
	char text[] = "Press button to start!";
	int color_code = 1;
	u8g2_ClearBuffer(display);
	u8g2_SetDrawColor(display, color_code);
	u8g2_SetFont(display, u8g2_font_6x10_tr);
	u8g2_DrawStr(display, (128 - u8g2_GetStrWidth(display, text))/2, 40, text);
	u8g2_SendBuffer(display);

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
	// Auf Knopfdruck warten
	char button_pressed = 0;
	while (!button_pressed) {
		if (isPressed(BUTTON_RIGHT)) {
			button_pressed = 1;
			while ( isPressed(BUTTON_RIGHT) );
		}
		HAL_Delay(20);
	}


}

char button_pressed = 0;

void loop(){
	current_cell = &map[current_pos[0]][current_pos[1]];
	// Auf Knopfdruck warten
	while (!button_pressed) {
		if (isPressed(BUTTON_RIGHT)) {
			button_pressed = 1;
			while ( isPressed(BUTTON_RIGHT) );
		}
		HAL_Delay(20);
	}

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

		while (1) {

		}
	}

	// Mit Ultraschall schauen, wo Wände sind und in Karte speichern
	checkForWalls();

	// Falls noch nicht bewegt, einmal um 90° drehen und erneut scannen, um auch die hintere Wand erkennen zu können
	if (movement_counter == 0) {
		turn90(0);
		orientation = 2;
		checkForWalls();
		turn90(1);
		orientation = 0;
	}

	// Feld auf Display ausgeben
	displayCell();

	if (movement_counter == 0){
		next_direction = lowestCounterDirection();
	} else {
		next_direction = getTremauxDirection();
	}
	// Entscheidung auf Bilschirm ausgeben und Aufforderung, ins nächste Feld platziert zu werden
	displayMovementDecision();

	// Zum nächsten Feld navigieren
	moveToNextField(next_direction);

	// Orientierung aktualisieren. Orientierung == Richtung, in die Zuletzt gefahren wurde
	orientation = next_direction;

	// Nächste Position bestimmen
	updateNextPosition();

	// Nächstes Feld bestimmen
	next_cell = &map[next_pos[0]][next_pos[1]];


	// Tremaux Counter aktualisieren
	updateTremauxCounters();

	// Position aktualisieren
	current_pos[0] = next_pos[0];
	current_pos[1] = next_pos[1];

	// Anzahl der zurückgelegten Felder aktualisieren
	movement_counter ++;
}


