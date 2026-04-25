#include "medium.h"
#include "ai_common.h"
#include "core/pile.h"
#include "core/deck.h"

static const uint8_t mediumMagicPriority[] = {3, 10, 2};

cardSelection aiMediumSelectPlay(const game* g, gameSettings s, const card* cards, uint8_t count, card top) {
	(void)g;
	cardSelection sel = findLowestValidSetMaxQuantity(cards, count, top, s);
	if (!selectionIsEmpty(sel)) return sel;
	return findMagicCard(cards, count, top, s, mediumMagicPriority, 3);
}
