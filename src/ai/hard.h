#ifndef HARD_H
#define HARD_H

#include "ai_common.h"
#include "game.h"

cardSelection aiHardSelectPlay(const game* g, gameSettings s, const card* cards, uint8_t count, card top);

#endif
