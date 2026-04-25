#include "hard.h"
#include "ai_common.h"
#include "core/pile.h"
#include "core/deck.h"

static const uint8_t hardMagicPriority[] = {2, 3, 10};

cardSelection aiHardSelectPlay(const game* g, gameSettings s, const card* cards, uint8_t count, card top) {
	cardSelection sel = tryCompleteFourOfAKind(g, cards, count, top, s);
	if (!selectionIsEmpty(sel)) return sel;

	sel = forcePickupFromKnown(cards, count, g->aiKnownPlayerCards, g->stats.aiKnownPlayerCardsCounter, top, s);
	if (!selectionIsEmpty(sel)) return sel;

	sel = findLowestStandardValidSingle(cards, count, top, s);
	if (!selectionIsEmpty(sel)) return sel;

	return findMagicCard(cards, count, top, s, hardMagicPriority, 3);
}
