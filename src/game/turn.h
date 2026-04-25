#ifndef TURN_H
#define TURN_H

#include "game.h"

uint8_t runGameLoop(game* g, gameSettings s, uint8_t slot);
uint8_t applyMagicEffects(game* g, uint8_t playedValue, gameSettings s);
uint8_t applyThreeOnEightBonus(game* g, gameSettings s);
uint8_t resolveAnotherTurn(game* g, uint8_t playedValue, gameSettings s);

#endif
