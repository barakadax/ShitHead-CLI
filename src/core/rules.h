#ifndef RULES_H
#define RULES_H

#include "card_rank.h"
#include "game.h"

uint8_t canPlayCardOn(card c, playContext ctx);
cardIndexList legalCardIndices(cardSet cards, playContext ctx);
uint8_t hasAnyLegalPlay(cardSet cards, playContext ctx);

#endif
