#ifndef PILE_H
#define PILE_H

#include "game.h"

void pushCardsToPile(game* g, const card* cards, uint8_t count);
void burnPile(game* g);
void pickupPileIntoHand(game* g, uint8_t isAi);
void addPileToKnownPlayerCards(game* g);
card topEffectiveCard(const game* g);
uint8_t countConsecutiveTopMatches(const game* g);

#endif
