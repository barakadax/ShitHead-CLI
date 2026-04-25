#ifndef SAVE_H
#define SAVE_H

#include "game.h"

#define saveSlotCount 3

uint8_t slotExists(uint8_t slot);
uint8_t slotIsFinished(uint8_t slot);
int saveSlot(uint8_t slot, const game* g, gameSettings settings, uint8_t gameOver);
int loadSlot(uint8_t slot, game* g, gameSettings* settings);

#endif
