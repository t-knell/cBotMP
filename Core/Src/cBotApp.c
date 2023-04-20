/*
 * Algo
 * Dieses Programm ist ein erster Prototyp für die Navigation durch das Labyrinth
 * mit Hilfe des Trémaux Algorithmus.
 * Der Roboter fährt nicht selbst, sondern analysiert nach Knopfdruck nur das Feld
 * in dem er gerade steht und gibt dann auf dem Bildschirm aus, wohin er als nächstes fahren möchte.
 * Wäre auch cool, wenn er bei der Analyse immer angeben würde, was er gerade um sich herum "sieht" -> Counter ausgeben
 */


#include "cBotApp.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

typedef struct {
	// Braucht jetzt 4 Byte, könnte man theoretisch auf 2 Byte reduzieren, aber super viel Hustle
	// Wand = 10, nicht durchfahren & offen = 0, 1 mal durchfahren = 1, 2 mal durchfahren = 2
	char counter_n;
	char counter_e;
	char counter_s;
	char counter_w;
} cell;

const cell base_cell = {0, 0, 0, 0};
cell map[15][15];
// Momentane Position im Feld
// Momentane Orientierung NSEW
// Anzahl der zurückgelegten Felder


void init(){
	// LEDs setzen
	clearLeds();
	int bright_white = getColorHSV(0, 0, 80);
	int calm_white = getColorHSV(0,0,10);
	for (int i; i<8; i++){
		setLed(i, calm_white);
	}
	setLed(8, bright_white);
	setLed(9, bright_white);
	updateLeds();

	// Initialisierung der Map mit der Standardzelle
}


void loop(){
	// Auf Knopfdruck warten
		// Check ob am Ziel angekommen
			//Falls ja, Bildschirmausgabe
		// Falls erste Bewegung, dann random ein Feld wählen
		// Alle 3 Ultraschallsensor Signale einlesen. Wenn Abstand < Schwelle, dann ist da eine Wand
		// Map aktualisieren
		// Wenn nur der Weg offen ist (Counter <=1), durch den wir gerade gefahren sind, dann durch diesen fahren und den Counter aktualisieren
		// Wenn zwei Wege offen sind, dann den nehmen, durch den wir nicht gefahren sind -> Pfad geht weiter
		// Wenn mehr als zwei Wege offen sind und wir das erste Mal an dieser Kreuzung sind, dann einen zufälligen, nicht befahrenen Weg wählen & Counter aktualisieren
		// Wenn mehr als zwei Wege offen sind, aber andere Wege in der Kreuzung markiert sind und der eben durchfahrene Weg keinen Counter=2 Wert hat, dann wieder umkehren
		// Wenn mehr als zwei offen sind, die Kreuzung schon Markierungen hat und der eben durchfahrene Weg Counter=2 hat, dann den Weg mit den niedrigsten Markierungen wählen. Falls gleichstand, dann random einen wählen
		// Wenn das alles nicht der Fall ist, dann Selbstzerstörung
	// Entscheidung auf Bilschirm ausgeben und Aufforderung, ins nächste Feld platziert zu werden
	// Position aktualisieren
	// Orientierung aktualisieren. Orientierung == Richtung, in die Zuletzt gefahren wurde
	// Tremaux Counter aktualisieren
	// Anzahl der zurückgelegten Felder aktualisieren

}
