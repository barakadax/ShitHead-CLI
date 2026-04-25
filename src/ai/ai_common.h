#ifndef AI_COMMON_H
#define AI_COMMON_H

#include "game.h"
#include "core/rules.h"

typedef struct cardSelection {
	uint8_t cardValue;
	uint8_t count;
	uint8_t indices[maxCardsAmount];
} cardSelection;

cardSelection emptySelection(void);
uint8_t selectionIsEmpty(cardSelection sel);

cardSelection findHighestStandardValidSingle(cardSet hand, playContext ctx);
cardSelection findLowestStandardValidSingle(cardSet hand, playContext ctx);
cardSelection findLowestValidSetMaxQuantity(cardSet hand, playContext ctx);
cardSelection findMagicCard(cardSet hand, playContext ctx, valueList priority);
cardSelection tryCompleteFourOfAKind(const game* g, cardSet hand, playContext ctx);
cardSelection forcePickupFromKnown(cardSet hand, cardSet known, playContext ctx);

uint8_t removeCardIndices(card* hand, uint8_t handCount, cardIndexList indices);

#endif
