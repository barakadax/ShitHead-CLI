#include "medium.h"
#include "ai_common.h"

static const uint8_t mediumMagicPriority[] = {3, 10, 2};

cardSelection aiMediumSelectPlay(const game* g, cardSet hand, playContext ctx) {
	(void)g;
	cardSelection sel = findLowestValidSetMaxQuantity(hand, ctx);
	if (!selectionIsEmpty(sel)) return sel;
	return findMagicCard(hand, ctx, (valueList){mediumMagicPriority, 3});
}
