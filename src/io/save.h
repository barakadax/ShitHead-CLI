#ifndef SAVE_H
#define SAVE_H

#include "game.h"

#define saveSlotCount 3

typedef struct slotSnapshot {
	const game* g;
	gameSettings settings;
	uint8_t gameOver;
} slotSnapshot;

uint8_t slotExists(uint8_t slot);
uint8_t slotIsFinished(uint8_t slot);
int saveSlot(uint8_t slot, slotSnapshot snap);
int loadSlot(uint8_t slot, game* g, gameSettings* settings);

#endif
