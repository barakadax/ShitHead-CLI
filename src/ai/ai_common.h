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

cardSelection findHighestStandardValidSingle(const card* hand, uint8_t handCount, card top, gameSettings s);
cardSelection findLowestStandardValidSingle(const card* hand, uint8_t handCount, card top, gameSettings s);
cardSelection findLowestValidSetMaxQuantity(const card* hand, uint8_t handCount, card top, gameSettings s);
cardSelection findMagicCard(const card* hand, uint8_t handCount, card top, gameSettings s, const uint8_t* priorityValues, uint8_t priorityCount);
cardSelection tryCompleteFourOfAKind(const game* g, const card* hand, uint8_t handCount, card top, gameSettings s);
cardSelection forcePickupFromKnown(const card* hand, uint8_t handCount, const card* knownPlayerCards,
	uint8_t knownCount, card top, gameSettings s);

uint8_t removeCardIndicesFromHand(card* hand, uint8_t handCount, const uint8_t* indices, uint8_t indexCount);

#endif
