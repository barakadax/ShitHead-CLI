#ifndef RULES_H
#define RULES_H

#include "card_rank.h"
#include "game.h"

uint8_t canPlayCardOn(card c, card top, gameSettings s);
void legalCardIndicesFromHand(const card* hand, uint8_t handCount, card top, gameSettings s, uint8_t* outIndices, uint8_t* outCount);
void legalCardIndicesFromFaceUp(const card* faceUp, uint8_t faceUpCount, card top, gameSettings s, uint8_t* outIndices, uint8_t* outCount);
uint8_t hasAnyLegalPlayInHand(const card* hand, uint8_t handCount, card top, gameSettings s);

#endif
