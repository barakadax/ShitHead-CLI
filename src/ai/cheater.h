#ifndef CHEATER_H
#define CHEATER_H

#include "ai_common.h"
#include "game.h"

cardSelection aiCheaterSelectPlay(const game* g, gameSettings s, const card* cards, uint8_t count, card top);

#endif
