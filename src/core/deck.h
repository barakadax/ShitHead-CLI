#ifndef DECK_H
#define DECK_H

#include "game.h"

void initGame(game* g, gameSettings settings);
void initHandCards(game* g, gameSettings settings);
void compactDeck(card* deck);
int compareCards(const void* a, const void* b);
void sortHands(game* g, gameSettings settings, uint8_t forcePlayerSort);
void sortSingleHand(card* hand, uint8_t count);
void drawUpToThreeForPlayer(game* g, gameSettings settings);
void drawUpToThreeForAi(game* g, gameSettings settings);

#endif
