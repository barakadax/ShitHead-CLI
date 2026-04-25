#ifndef MEDIUM_H
#define MEDIUM_H

#include "ai_common.h"
#include "game.h"

cardSelection aiMediumSelectPlay(const game* g, gameSettings s, const card* cards, uint8_t count, card top);

#endif
