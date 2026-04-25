#include "hard.h"
#include "ai_common.h"

static const uint8_t hardMagicPriority[] = {2, 3, 10};

cardSelection aiHardSelectPlay(const game* g, cardSet hand, playContext ctx) {
	cardSelection sel = tryCompleteFourOfAKind(g, hand, ctx);
	if (!selectionIsEmpty(sel)) return sel;

	cardSet known = {g->aiKnownPlayerCards, g->stats.aiKnownPlayerCardsCounter};
	sel = forcePickupFromKnown(hand, known, ctx);
	if (!selectionIsEmpty(sel)) return sel;

	sel = findLowestStandardValidSingle(hand, ctx);
	if (!selectionIsEmpty(sel)) return sel;

	return findMagicCard(hand, ctx, (valueList){hardMagicPriority, 3});
}
