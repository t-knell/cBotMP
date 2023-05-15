/*
 * Display Mapping
 * Auf dem Display die Karte anzeigen
 */

#include "cBotApp.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
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
int next_direction = NORTH; // Richtungsanweisung für die nächste Bewegung
int moves = 0; // Anzahl der zurückgelegten Felder


// Zeigt, wie das Display gedreht werden kann
void turnedDisplay() {
	u8g2_ClearBuffer(display);
	u8g2_SetDisplayRotation(display, U8G2_R2);
	char text[] = "T";
	u8g2_SetDrawColor(display, 1);
	u8g2_SetFont(display, u8g2_font_fub20_tf); // u8g2_font_10x20_mr) / u8g2_font_t0_22b_mr / u8g2_font_crox5tb_tf
	u8g2_DrawStr(display, (128 - u8g2_GetStrWidth(display, text))/2, (64 + u8g2_GetAscent(display))/2, text);
	u8g2_SendBuffer(display);
}


// Display initialisieren
void initDisplay() {
	int color_code = 1;
	u8g2_ClearBuffer(display);
	u8g2_SetDrawColor(display, color_code);
	u8g2_SetFont(display, u8g2_font_6x10_tr);
}

// Gibt eine Zeile Text auf dem Bildschirm aus
void printToDisplay(char text[], int pos_x, int pos_y){
	int color_code = 1;
	u8g2_ClearBuffer(display);
	u8g2_SetDrawColor(display, color_code);
	u8g2_SetFont(display, u8g2_font_6x10_tr);
	u8g2_DrawStr(display, pos_x, pos_y, text);
	u8g2_SendBuffer(display);
}


// Konstanter Punkt bei Drehung
void displayCenterCircle() {
	int center_x;
	int center_y;
	int radius = 1;
	if (current_direction == NORTH || current_direction == SOUTH) {
		center_x = 63;
		center_y = 35;
	} else if (current_direction == EAST && current_direction == WEST) {
		center_x = 35;
		center_y = 63;
	}
	u8g2_DrawDisc(display, center_x, center_y, radius, U8G2_DRAW_ALL);
}


//
void displayCell(cell* target_cell, int center_x, int center_y) {
	int line_length = 10;
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

void displayMap() {
	displayCenterCircle();

	cell* temp_cell;
	int pos_x, pos_y, distance_x, distance_y;
	int center_x = 63;
	int center_y = 35;
	int line_length = 10;
	for (int i = 0; i<15; i++){
		for (int j = 0; j<15; j++){
			if (abs(i-current_pos[0]) > 2) {
				continue;
			}
			if (abs(j-current_pos[1]) >5) {
				continue;
			}

			temp_cell = &map[i][j];
			distance_x = (j - current_pos[1]) * line_length; // Pixel
			distance_y = (i - current_pos[0]) * line_length; // Pixel
			pos_x = center_x + distance_x;
			pos_y = center_y + distance_y;
			displayCell(temp_cell, pos_x, pos_y);
		}
	}
	u8g2_SendBuffer(display);
}




void init() {
	// Setup für das Mapping
	char base_cell[] = {9, 9, 9, 9};
	for (int i = 0; i<15; i++){
		for (int j = 0; j<15; j++){
			map[i][j].counter_n = base_cell[NORTH];
			map[i][j].counter_s = base_cell[SOUTH];
			map[i][j].counter_e = base_cell[EAST];
			map[i][j].counter_w = base_cell[WEST];
		}
	}
	current_cell = &map[current_pos[0]][current_pos[1]];

	initDisplay();
}

void loop() {
	displayMap();
}
