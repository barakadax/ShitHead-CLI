#include "cheater.h"
#include "ai_common.h"

static const uint8_t cheaterMagicPriority[] = {2, 3, 10};

static cardSelection forcePickupFromPlayerDirect(cardSet hand, const game* g, playContext ctx) {
	card combined[maxCardsAmount];
	uint8_t totalPlayerCards = 0;
	for (uint8_t i = 0; i < g->stats.playerHandCounter; i++) {
		combined[totalPlayerCards++] = g->player.hand[i];
	}
	for (uint8_t i = 0; i < g->stats.playerFaceUpCounter; i++) {
		combined[totalPlayerCards++] = g->player.faceUpCards[i];
	}
	for (uint8_t i = 0; i < g->stats.playerFaceDownCounter; i++) {
		combined[totalPlayerCards++] = g->player.faceDownCards[i];
	}
	return forcePickupFromKnown(hand, (cardSet){combined, totalPlayerCards}, ctx);
}

cardSelection aiCheaterSelectPlay(const game* g, cardSet hand, playContext ctx) {
	cardSelection sel = tryCompleteFourOfAKind(g, hand, ctx);
	if (!selectionIsEmpty(sel)) return sel;

	sel = forcePickupFromPlayerDirect(hand, g, ctx);
	if (!selectionIsEmpty(sel)) return sel;

	sel = findLowestStandardValidSingle(hand, ctx);
	if (!selectionIsEmpty(sel)) return sel;

	return findMagicCard(hand, ctx, (valueList){cheaterMagicPriority, 3});
}
