/*
 * Labyrinth Bot
 * Dies ist das Hauptprogramm für den LabyrinthBot. Das Programm ermöglicht dem Roboter das eigenständige Lösen eines
 * Labyrinths mit 90° und 180° Winkeln. Der Nutzer hat die Möglichkeit zwischen drei unterschiedlichen Algorithmen zu wählen
 * um das Labyrinth zu lösen: Zufällig, Rechte Wand und Trémaux. Nach erfolreichen Lösen des Labyrinths vollzieht der
 * Roboter einen Siegestanz.
 */

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "cBotApp.h"
#include "marioSong.h"

enum orientation {
	NORTH,
	EAST,
	SOUTH,
	WEST
};

// 4 Zahlen pro Zelle, die die Himmelrichtungen abbilden. 0 = frei & unbefahren, 1 = 1x befahren, ..., 9 = Wand
char map[15][15][4] = {0};
int current_pos[2] = {7,7};
int next_pos[2] = {7,7};
char direction = NORTH;
char next_direction = NORTH;
unsigned char moves = 0;


// Gibt die aktuelle Zelle zurück
char* getCurrentCell() {
	return map[current_pos[0]][ current_pos[1]];
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


// Gibt den Durchschnittswert mehrerer Ultraschall Distanzmessungen zurück
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


// Überprüft, ob vor dem Sensor eine Wand ist
char checkForWall(sensorId id) {
	int threshold = 150; //mm
	int distance = getAvgRangeMm(id);
	if (distance <= threshold) {
		return 1;
	}
	return 0;
}


char getSensorDirection(sensorId id) {
	if (orientation == 0) {

	}
}

// Aktualisiert die Wände in der aktuellen Zelle
void updateWalls(){
	char* walls_nesw[4] = {0};



}






void init() {

}

void loop () {

}
