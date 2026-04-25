#ifndef EASY_H
#define EASY_H

#include "ai_common.h"
#include "game.h"

cardSelection aiEasySelectPlay(const game* g, gameSettings s, const card* cards, uint8_t count, card top);

#endif
