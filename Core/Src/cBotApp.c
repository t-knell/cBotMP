/*
 * Labyrinth Bot + Mapping + Algorithmusauswahl
 * Dieses Programm lässt den Roboter eigenständig ein beliebiges Labyrinth (aus 250mm langen Wänden in 90° oder 180° Winkeln) lösen
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "cBotApp.h"
#include "marioSong.h"

enum direction {
	NORTH,
	SOUTH,
	EAST,
	WEST,
};

enum turn_direction {
	RIGHT,
	LEFT
};

enum algos {
	TREMAUX,
	RANDOM,
	WALL_FOLLOW
};

typedef struct {
	char counter_n;
	char counter_s;
	char counter_e;
	char counter_w;
} cell;

cell map[15][15];
int current_pos[2] = {7, 7}; // Momentane Position im Labyrinth - ROW - COL
int next_pos[2] = {7, 7};
cell *current_cell;
cell *next_cell;
char current_direction = NORTH;
char next_direction = NORTH; // Richtungsanweisung für die nächste Bewegung
int algo;
int moves = 0; // Anzahl der zurückgelegten Felder


// Display initialisieren
void initDisplay() {
	int color_code = 1;
	u8g2_ClearBuffer(display);
	u8g2_SetDrawColor(display, color_code);
	u8g2_SetFont(display, u8g2_font_6x10_tr);
}

// Gibt eine Zeile Text auf dem Bildschirm aus
void printToDisplay(char text[], int pos_x, int pos_y){
	u8g2_ClearBuffer(display);
	u8g2_SetDisplayRotation(display, U8G2_R0);
	u8g2_DrawStr(display, pos_x, pos_y, text);
	u8g2_SendBuffer(display);
}


// Gibt mehrere Zeilen Text auf dem Bildschirm aus
void multiPrintToDisplay(char** text, int n_strings, int start_x, int start_y){
	u8g2_ClearBuffer(display);
	u8g2_SetDisplayRotation(display, U8G2_R0);
	for (int i=0; i<n_strings; i++){
		u8g2_DrawStr(display, start_x, start_y, text[i]);
		start_y += 12;
	}
	u8g2_SendBuffer(display);
}


// Ermöglicht dem Nutzer, einen Algorithmus auszuwählen
int getAlgo() {
	char* text[] = {
			"Algorithmus Auswahl:",
			"Button links & rechts",
			"Bestaetigen:",
			"Unterer Button"
	};
	char* options[] = {
		"Tremaux Algorithmus",
		"Zufaellige Richtung",
		"Rechte Wand folgen"
	};
	int pos_x = 0;
	int pos_y = 20;
	multiPrintToDisplay(text, 4, pos_x, pos_y);

	pos_y += 20;
	int i = 10;
	while (1) {
		if (isPressed(BUTTON_RIGHT)) {
			i++;
			printToDisplay(options[i%3], pos_x, pos_y);
			while ( isPressed(BUTTON_RIGHT) );
		}
		if (isPressed(BUTTON_LEFT)) {
			if (i == 0) {
				i = 3;
			}
			i--;
			printToDisplay(options[i%3], pos_x, pos_y);
			while ( isPressed(BUTTON_LEFT) );
		}
		if (isPressed(BUTTON_DOWN)) {
			while ( isPressed(BUTTON_DOWN) );
			return i%3;
		}
	}
}


// Stellt aktuelle Position als Punkt dar
void displayCurrentPos(int pos_x, int pos_y) {
	int radius = 1;
	u8g2_DrawDisc(display, pos_x, pos_y, radius, U8G2_DRAW_ALL);
}


// Zeichnet eine Zelle auf Basis der x und y Koordinaten auf das Display
void displayCell(cell* target_cell, int center_x, int center_y) {
	int line_length = 20;
	int ll[] = {center_x - line_length / 2, center_y + line_length / 2}; //Ecke links unten (lower left)
	int lr[] = {center_x + line_length / 2, center_y + line_length / 2};
	int ul[] = {center_x - line_length / 2, center_y - line_length / 2};
	int ur[] = {center_x + line_length / 2, center_y - line_length / 2};

	if (target_cell->counter_n == 9){
		u8g2_DrawLine(display, ul[0], ul[1], ur[0], ur[1]);
	}
	if (target_cell->counter_e == 9){
		u8g2_DrawLine(display, ur[0], ur[1], lr[0], lr[1]);
	}
	if (target_cell->counter_s == 9){
		u8g2_DrawLine(display, lr[0], lr[1], ll[0], ll[1]);
	}
	if (target_cell->counter_w == 9){
		u8g2_DrawLine(display, ll[0], ll[1], ul[0], ul[1]);
	}
}


// Gibt die Karte in gewünschter Orientirung auf dem Display aus
void displayMap(enum direction display_direction) {
	u8g2_ClearBuffer(display);

	cell* temp_cell;
	int pos_x, pos_y, distance_x, distance_y;
	int center_x;
	int center_y;
	int line_length = 20;
	for (int i = 0; i<15; i++){
		for (int j = 0; j<15; j++){
			temp_cell = &map[i][j];
			if (display_direction == NORTH){
				u8g2_SetDisplayRotation(display, U8G2_R0);
				center_x = 63;
				center_y = 35;
			} else if (display_direction == SOUTH){
				u8g2_SetDisplayRotation(display, U8G2_R2);
				center_x = 63;
				center_y = 35;
			} else if (display_direction == EAST) {
				u8g2_SetDisplayRotation(display, U8G2_R3);
				center_x = 35;
				center_y = 63;
			} else {
				u8g2_SetDisplayRotation(display, U8G2_R1);
				center_x = 35;
				center_y = 63;
			}
			displayCurrentPos(center_x, center_y);
			distance_x = (j - current_pos[1]) * line_length; // Pixel
			distance_y = (i - current_pos[0]) * line_length; // Pixel
			pos_x = center_x + distance_x;
			pos_y = center_y + distance_y;
			displayCell(temp_cell, pos_x, pos_y);
		}
	}
	u8g2_SendBuffer(display);
}


// Bringt den Roboter in einen stabilen Error Zustand und gibt eine Benachrichtigung auf dem Display aus
void goToErrorState() {
	// Motor anhalten
	int rpm_left = 0;
	int rpm_right = 0;
	setMotorRpm(rpm_left, rpm_right);

	char* strings[] = {
		"Programmfehler!",
		"Reset druecken, um",
		"Programm neuzustarten"
	};
	int pos_x = 0;
	int pos_y = 30;
	multiPrintToDisplay(strings, 3, pos_x, pos_y);
	while (1){
	}

}


// Überprüft, ob das Ziel erreicht wurde
char goalReached(){
	int counter = 0;
	int sleep_duration = 20;
	int value_threshold = 200;
	int time_threshold = 500; //in ms
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


// Gibt einen Durchschnittswert der Ultraschall-Distanzmessung zurück
int getAvgRangeMm(sensorId id) {
	int distance;
	int n = 5;
	int sum = 0;
	int average;
	char counter = 0;
	for (int i = 0; i < n; i++) {
		distance = getRangeMm(id);
		while (distance > 1000 || counter < 5) {
			distance = getRangeMm(id);
			counter ++;
			HAL_Delay(1);
		}
		sum += distance;
		HAL_Delay(1);
	}
	average = sum / n;
	return average;
}


// Sucht nach Wänden um den Roboter und aktualisiert anschließend die Counter der aktuellen Zelle
void checkForWalls(){
	int wall_threshold = 150; // mm
	int sensor_values[] = {getAvgRangeMm(SENSOR_LEFT), getAvgRangeMm(SENSOR_MIDDLE), getAvgRangeMm(SENSOR_RIGHT)};
	int sensor_values_nsew[4];

	// Sensor Werte in ihre Himmelsrichtung Orientierung umtragen
	if (current_direction == NORTH){
		sensor_values_nsew[NORTH] = sensor_values[1];
		sensor_values_nsew[SOUTH] = -1; // Von da ist der Roboter gerade gekommen
		sensor_values_nsew[EAST] = sensor_values[2];
		sensor_values_nsew[WEST] = sensor_values[0];
	} else if(current_direction == SOUTH){
		sensor_values_nsew[NORTH] = -1;
		sensor_values_nsew[SOUTH] = sensor_values[1];
		sensor_values_nsew[EAST] = sensor_values[0];
		sensor_values_nsew[WEST] = sensor_values[2];
	} else if(current_direction == EAST){
		sensor_values_nsew[NORTH] = sensor_values[0];
		sensor_values_nsew[SOUTH] = sensor_values[2];
		sensor_values_nsew[EAST] = sensor_values[1];
		sensor_values_nsew[WEST] = -1;
	} else if(current_direction == WEST){
		sensor_values_nsew[NORTH] = sensor_values[2];
		sensor_values_nsew[SOUTH] = sensor_values[0];
		sensor_values_nsew[EAST] = -1;
		sensor_values_nsew[WEST] = sensor_values[1];
	} else{
		goToErrorState();
	}

	// Überprüft, ob in den Himmelsrichtungen eine Wand ist und falls ja, dies in der Karte vermerken
	for (int i=0; i<4; i++) {
		if ((sensor_values_nsew[i] <= wall_threshold) && (sensor_values_nsew[i] >= 0)) {
			if (i == NORTH) {
				current_cell->counter_n = 9;
			} else if (i == SOUTH) {
				current_cell->counter_s = 9;
			} else if (i == EAST) {
				current_cell->counter_e	= 9;
			} else {
				current_cell->counter_w = 9;
			}
		}
	}
}


// Gibt eine Richtung mit dem niedrigsten erlaubten Counter Wert zurück
int getLowestCounterDirection(){
	for (int i = 0; i<2; i++) {
		if (current_cell->counter_n == i) {
			return NORTH;
		}
		if (current_cell->counter_s == i) {
			return SOUTH;
		}
		if (current_cell->counter_e == i) {
			return EAST;
		}
		if (current_cell->counter_w == i) {
			return WEST;
		}
	}
	goToErrorState();
	return -1;
}


// Gibt die Anzahl der offenen Wege zurück
char getNumberOfExits() {
	char counter = 0;
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
	return counter;
}


// Gibt die entgegengestzte Himmelsrichtung zurück
char getOppositeDirection(char direction) {
	if (direction == NORTH){
		return SOUTH;
	}
	if (direction == SOUTH){
		return NORTH;
	}
	if (direction == EAST){
		return WEST;
	}
	if (direction == WEST){
		return EAST;
	}
	goToErrorState();
	return -1;
}


// Überprüft, ob der Roboter schon in der Zelle war
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
	if (direction == NORTH){
		return current_cell->counter_n;
	}
	if (direction == SOUTH){
		return current_cell->counter_s;
	}
	if (direction == EAST){
		return current_cell->counter_e;
	}
	if (direction == WEST){
		return current_cell->counter_w;
	}
	goToErrorState();
	return -1;
}


// Gibt zurück, in welche Richtung laut dem Tremaux Algorithmus als nächstes gefahren werden soll
char getTremauxDirection() {
	char exits = getNumberOfExits();
	char opposite_direction = getOppositeDirection(current_direction);
	char lowest_counter_direction = getLowestCounterDirection();
	// Wenn nur der Weg offen ist (Counter <= 1), durch den wir gerade gefahren sind, dann durch diesen fahren und den Counter aktualisieren
	if (exits == 1){
		return lowest_counter_direction;
	}
	// Wenn nur zwei Wege offen sind, dann den Ausgang mit dem kleineren Counter wählen
	if (exits == 2) {
		return lowest_counter_direction;
	}
	// Wenn mehr als zwei Wege offen sind und die Zelle vorher noch nicht besucht wurde, eine der Zellen mit dem kleinsten Counter wählen
	if (!cellVisited()) {
		return lowest_counter_direction;
	}
	// Wenn mehr als zwei Wege offen und die Zelle schon besucht wurde, dann umkehren, außer dieser Ausgang wurde schon 2x benutzt
	char opposite_direction_counter = getCounterFromDirection(opposite_direction);
	if ( opposite_direction_counter < 2) {
		return opposite_direction;
	}
	// Wenn Ausgang schon 2x benutzt, dann einen Ausgang mit dem niedrigsten Counter wählen
	return lowest_counter_direction;
}


// Gibt eine zufäliige Richtung zurück, in der keine Wand ist
char getRandomDirection() {
	int random_direction;
	srand(getRangeMm(SENSOR_RIGHT));
	while (1) {
		random_direction= rand() % 4;
		if (random_direction == NORTH) {
			if (current_cell->counter_n < 9) {
				return random_direction;
			}
		}
		if (random_direction == SOUTH) {
			if (current_cell->counter_s < 9) {
				return random_direction;
			}
		}
		if (random_direction == EAST) {
			if (current_cell->counter_e < 9) {
				return random_direction;
			}
		}
		if (random_direction == WEST) {
			if (current_cell->counter_w < 9) {
				return random_direction;
			}
		}
		HAL_Delay(200);
	}
}


// Gibt einen Richtung zurück, die den Roboter die rechte Wand folgen lässt
char getWallFollowerDirection() {
	char wall_right = 0, wall_middle = 0, wall_left = 0;
	if (current_direction == NORTH) {
		if (current_cell->counter_e == 9) {
			wall_right = 1;
		}
		if (current_cell->counter_n == 9) {
			wall_middle = 1;
		}
		if (current_cell->counter_w == 9) {
			wall_left = 1;
		}
		if (wall_right && !wall_middle) {
			return NORTH;
		}
		if (wall_right && wall_middle && !wall_left) {
			return WEST;
		}
		if (!wall_right) {
			return EAST;
		}
		if (wall_right && wall_middle && wall_left) {
			return SOUTH;
		}
	}
	if (current_direction == SOUTH) {
		if (current_cell->counter_w == 9) {
			wall_right = 1;
		}
		if (current_cell->counter_s == 9) {
			wall_middle = 1;
		}
		if (current_cell->counter_e == 9) {
			wall_left = 1;
		}
		if (wall_right && !wall_middle) {
			return SOUTH;
		}
		if (wall_right && wall_middle && !wall_left) {
			return EAST;
		}
		if (!wall_right) {
			return WEST;
		}
		if (wall_right && wall_middle && wall_left) {
			return NORTH;
		}
	}
	if (current_direction == EAST) {
		if (current_cell->counter_s == 9) {
			wall_right = 1;
		}
		if (current_cell->counter_e == 9) {
			wall_middle = 1;
		}
		if (current_cell->counter_n == 9) {
			wall_left = 1;
		}
		if (wall_right && !wall_middle) {
			return EAST;
		}
		if (wall_right && wall_middle && !wall_left) {
			return NORTH;
		}
		if (!wall_right) {
			return SOUTH;
		}
		if (wall_right && wall_middle && wall_left) {
			return WEST;
		}
	}
	if (current_direction == WEST) {
		if (current_cell->counter_n == 9) {
			wall_right = 1;
		}
		if (current_cell->counter_w == 9) {
			wall_middle = 1;
		}
		if (current_cell->counter_s == 9) {
			wall_left = 1;
		}
		if (wall_right && !wall_middle) {
			return WEST;
		}
		if (wall_right && wall_middle && !wall_left) {
			return SOUTH;
		}
		if (!wall_right) {
			return NORTH;
		}
		if (wall_right && wall_middle && wall_left) {
			return EAST;
		}
	}
	goToErrorState();
	return -1;
}


// Aktualisiert die Tremaux Counter
void updateTremauxCounters() {
	if (current_direction == NORTH){
		current_cell->counter_n += 1;
		next_cell->counter_s += 1;
	}
	if (current_direction == SOUTH){
		current_cell->counter_s += 1;
		next_cell->counter_n += 1;
	}
	if (current_direction == EAST){
		current_cell->counter_e += 1;
		next_cell->counter_w += 1;
	}
	if (current_direction == WEST){
		current_cell->counter_w += 1;
		next_cell->counter_e += 1;
	}
}


// Berechnet die nächste Position und speichert sie ab
void updateNextPosition() {
	if (next_direction == NORTH) {
		next_pos[0] -= 1;
	} else if (next_direction == SOUTH) {
		next_pos[0] += 1;
	} else if (next_direction == EAST) {
		next_pos[1] += 1;
	} else if (next_direction == WEST) {
		next_pos[1] -= 1;
	}
}


// Dreht den Roboter um 90° in die gewünschte Richtung
void turn90(char turn_direction) {
	int rpm_left, rpm_right;
	if (turn_direction == RIGHT) {
		rpm_left = 15;
		rpm_right = -15;
	} else if (turn_direction == 1) {
		rpm_left = -15;
		rpm_right = 15;
	}
	setMotorRpm(rpm_left, rpm_right);
	HAL_Delay(1950);
	rpm_left = 0;
	rpm_right = 0;
	setMotorRpm(rpm_left, rpm_right);
}


// Dreht den Roboter um 180°
void turn180() {
	int rpm_left = -15, rpm_right = 15;
	setMotorRpm(rpm_left, rpm_right);
	HAL_Delay(3880);
	rpm_left = 0;
	rpm_right = 0;
	setMotorRpm(rpm_left, rpm_right);
}


// Lässt den Roboter ein Feld vorwärts fahren
void moveOneFieldForward() {
	int rpm_left = 15, rpm_right = 15;
	setMotorRpm(rpm_left, rpm_right);
	HAL_Delay(5250);
	rpm_left = 0;
	rpm_right = 0;
	setMotorRpm(rpm_left, rpm_right);
}


// Ändert Roboter Orientierung zur übergebenen Himmelsrichtung
void turnToDirection(char direction) {
	if (current_direction == NORTH) {
		if (direction == NORTH) {
			return;
		}
		if (direction == SOUTH) {
			turn180();
		} else if (direction == WEST) {
			turn90(LEFT);
		} else if (direction == EAST) {
			turn90(RIGHT);
		}
	} else if (current_direction == SOUTH) {
		if (direction == SOUTH) {
			return;
		}
		if (direction == NORTH) {
			turn180();
		} else if (direction == EAST) {
			turn90(LEFT);
		} else if (direction == WEST) {
			turn90(RIGHT);
		}
	} else if (current_direction == EAST) {
		if (direction == EAST) {
			return;
		}
		if (direction == WEST) {
			turn180();
		} else if (direction == NORTH) {
			turn90(LEFT);
		} else if (direction == SOUTH) {
			turn90(RIGHT);
		}
	} else if (current_direction == WEST) {
		if (direction == WEST) {
			return;
		}
		if (direction == EAST) {
			turn180();
		} else if (direction == SOUTH) {
			turn90(LEFT);
		} else if (direction == NORTH) {
			turn90(RIGHT);
		}
	}
}


// Manövriert den Roboter in das nächste Feld
void moveToNextField(char direction) {
	if (direction == current_direction) {
		moveOneFieldForward();
	} else {
		turnToDirection(direction);
		// Karte an Orientierung anpassen
		displayMap(direction);
		moveOneFieldForward();
	}
}


// Richtet den Roboter im Feld neu aus, sobald der Roboter zu nah an eine Wand kommt
void adjustPosition() {
	int rpmLeft;
	int rpmRight;
	int side_threshold = 70;
	int front_threshold = 70;

	int sensor_left = getAvgRangeMm(SENSOR_LEFT);
	int sensor_right = getAvgRangeMm(SENSOR_RIGHT);
	int sensor_middle = getAvgRangeMm(SENSOR_MIDDLE);

	char first_correction = 1;

	while ((sensor_right < side_threshold) || (sensor_left < side_threshold) || (sensor_middle < front_threshold)) {
		int turn_back_delay;
		// Beim ersten Korrekturzug kürzer zurück drehen als ursprüngliche Drehung, um Schrägheit auszugleichen
		if (first_correction == 1) {
			turn_back_delay = 200;
			first_correction = 0;
		// Bei mehreren Korrekturzügen muss das Zurückdrehen angepasst werden, da der Roboter sonst überkorrigiert
		} else {
			turn_back_delay = 300;
		}

		sensor_right = getRangeMm(SENSOR_RIGHT);

		if(sensor_right < side_threshold ) {
			rpmLeft = -10;
			rpmRight = -10;
			setMotorRpm(rpmLeft, rpmRight);
			HAL_Delay(2500);
			rpmLeft = -10;
			rpmRight = 10;
			setMotorRpm(rpmLeft, rpmRight);
			HAL_Delay(300);
			rpmLeft = 10;
			rpmRight = 10;
			setMotorRpm(rpmLeft, rpmRight);
			HAL_Delay(2500);
			rpmLeft = 10;
			rpmRight = -10;
			setMotorRpm(rpmLeft, rpmRight);
			HAL_Delay(turn_back_delay);
			rpmLeft = 0;
			rpmRight = 0;
			setMotorRpm(rpmLeft, rpmRight);
		}

		sensor_left = getRangeMm(SENSOR_LEFT);

		if(sensor_left < side_threshold ) {
			rpmLeft = -10;
			rpmRight = -10;
			setMotorRpm(rpmLeft, rpmRight);
			HAL_Delay(2500);
			rpmLeft = 10;
			rpmRight = -10;
			setMotorRpm(rpmLeft, rpmRight);
			HAL_Delay(300);
			rpmLeft = 10;
			rpmRight = 10;
			setMotorRpm(rpmLeft, rpmRight);
			HAL_Delay(2500);
			rpmLeft = -10;
			rpmRight = 10;
			setMotorRpm(rpmLeft, rpmRight);
			HAL_Delay(turn_back_delay);
			rpmLeft = 0;
			rpmRight = 0;
			setMotorRpm(rpmLeft, rpmRight);
		}

		sensor_middle = getRangeMm(SENSOR_MIDDLE);

		if(sensor_middle < front_threshold ) {
			rpmLeft = -10;
			rpmRight = -10;
			setMotorRpm(rpmLeft, rpmRight);
			HAL_Delay(600);
			rpmLeft = 0;
			rpmRight = 0;
			setMotorRpm(rpmLeft, rpmRight);
		}

	}
}


// Initialisiert einen Siegestanz inklusive SuperMario Musik und blinkenden LEDs
void victoryDance() {
	int hueStart = 0;
	int rpmLeft = 0;
	int rpmRight = 0;

	hueStart = (hueStart - 12 + 1536) % 1536;
	for ( int i = 0; i < 10; i++ ) {
		setLed(i, getColorHSV((hueStart + 128 * i) % 1536, 50, 0));
	}
	updateLeds();

	playMario();

	if (isPlaying()){
		while (isPlaying()) {
		rpmLeft = 10;
		rpmRight = -10;
		setMotorRpm(rpmLeft, rpmRight);
		for ( int a = 0; a < 5; a++) {
			for ( int i = 0; i < 10; i++) {
				setLed(i, getColorHSV((hueStart + 100 * a) % 1536, 100, 10));
				setLed(i+2, getColorHSV((hueStart + 200 * a) % 1536, 100, 10));
				setLed(i+4, getColorHSV((hueStart + 300 * a) % 1536, 100, 10));
				setLed(i+6, getColorHSV((hueStart + 400 * a) % 1536, 100, 10));
				HAL_Delay(50);
			}
		}

		rpmLeft = -10;
		rpmRight = 10;
		setMotorRpm(rpmLeft, rpmRight);
		for ( int a = 0; a < 5; a++) {
			for ( int i = 0; i < 10; i++) {
				setLed(i, getColorHSV((hueStart + 100 * a) % 1536, 100, 10));
				setLed(i+1, getColorHSV((hueStart + 200 * a) % 1536, 100, 10));
				setLed(i+3, getColorHSV((hueStart + 300 * a) % 1536, 100, 10));
				setLed(i+5, getColorHSV((hueStart + 400 * a) % 1536, 100, 10));
				HAL_Delay(50);
			}
		}

		rpmLeft = 5;
		rpmRight = 5;
		setMotorRpm(rpmLeft, rpmRight);
		for ( int a = 0; a < 5; a++) {
			for ( int i = 0; i < 10; i++) {
				setLed(i, getColorHSV((hueStart + 100 * a) % 1536, 100, 10));
				setLed(i+1, getColorHSV((hueStart + 200 * a) % 1536, 100, 10));
				setLed(i+3, getColorHSV((hueStart + 300 * a) % 1536, 100, 10));
				setLed(i+5, getColorHSV((hueStart + 400 * a) % 1536, 100, 10));
				HAL_Delay(50);
			}
		}

		rpmLeft = -5;
		rpmRight = -5;
		setMotorRpm(rpmLeft, rpmRight);
		for ( int a = 0; a < 5; a++) {
			for ( int i = 0; i < 10; i++) {
				setLed(i, getColorHSV((hueStart + 100 * a) % 1536, 100, 10));
				setLed(i+1, getColorHSV((hueStart + 200 * a) % 1536, 100, 10));
				setLed(i+3, getColorHSV((hueStart + 300 * a) % 1536, 100, 10));
				setLed(i+5, getColorHSV((hueStart + 400 * a) % 1536, 100, 10));
				HAL_Delay(50);
			}
		}
		}
	}
	if (!isPlaying()) {
		rpmLeft = 0;
		rpmRight = 0;
		setMotorRpm(rpmLeft, rpmRight);
	}
	HAL_Delay(20);
}


void init(){
	// Display initialisieren
	int pos_x = 0;
	int pos_y = 20;
	initDisplay();

	// Startanweisungen, Algorithmusauswahl
	algo = getAlgo();

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

	char base_cell[] = {0, 0, 0, 0};
	for (int i = 0; i<15; i++){
		for (int j = 0; j<15; j++){
			map[i][j].counter_n = base_cell[NORTH];
			map[i][j].counter_s = base_cell[SOUTH];
			map[i][j].counter_e = base_cell[EAST];
			map[i][j].counter_w = base_cell[WEST];
		}
	}
	current_cell = &map[current_pos[0]][current_pos[1]];

	char* strings2[] = {
			"Algo ausgewaehlt",
			"Loesung mit:",
			"Tremaux",
			"Algorithmus"
	};
	char* options[] = {
		"Tremaux-",
		"Zufaellige Richtung",
		"Rechte Wand folgen"
	};
	strings2[2] = options[algo];
	multiPrintToDisplay(strings2, 4, pos_x, pos_y);
}


void loop(){
	current_cell = &map[current_pos[0]][current_pos[1]];

	//Abstände zur Wand korrigieren
	adjustPosition();

	// Überprüfen, ob am Ziel angekommen
	if(goalReached()){
		char text[] = "Ziel erreicht!";
		int pos_x = (128 - u8g2_GetStrWidth(display, text)) / 2;
		int pos_y = (64 + u8g2_GetAscent(display))/2;
		printToDisplay(text, pos_x, pos_y);

		victoryDance();

		while (1) {
		}
	}

	// Mit Ultraschall schauen, wo Wände sind und in Karte speichern
	checkForWalls();

	// Falls noch nicht bewegt, einmal um 90° drehen und erneut scannen, um auch die hintere Wand mappen zu können
	if (moves == 0) {
		turn90(RIGHT);
		current_direction = EAST;
		checkForWalls();
		turn90(LEFT);
		current_direction = NORTH;
	}

	// Karte auf Display ausgeben
	displayMap(current_direction);

	if ((moves == 0) && (algo == TREMAUX)){
		next_direction = getLowestCounterDirection();
	} else {
		if (algo == TREMAUX) {
			next_direction = getTremauxDirection();
		} else if (algo == RANDOM) {
			next_direction = getRandomDirection();
		} else if (algo == WALL_FOLLOW) {
			 next_direction = getWallFollowerDirection();
		}

	}

	// Zum nächsten Feld navigieren
	moveToNextField(next_direction);

	// Orientierung aktualisieren. Orientierung == Richtung, in die Zuletzt gefahren wurde
	current_direction = next_direction;

	// Nächste Position bestimmen
	updateNextPosition();

	// Nächstes Feld bestimmen
	next_cell = &map[next_pos[0]][next_pos[1]];


	// Tremaux Counter aktualisieren
	if (algo == TREMAUX) {
		updateTremauxCounters();
	}

	// Position aktualisieren
	current_pos[0] = next_pos[0];
	current_pos[1] = next_pos[1];

	// Anzahl der zurückgelegten Felder aktualisieren
	moves ++;
}
