#include "cheater.h"
#include "ai_common.h"
#include "core/pile.h"
#include "core/deck.h"

static const uint8_t cheaterMagicPriority[] = {2, 3, 10};

static cardSelection forcePickupFromPlayerDirect(const card* hand, uint8_t handCount, const game* g,
	card top, gameSettings s) {
	uint8_t allPlayerCards[maxCardsAmount];
	uint8_t totalPlayerCards = 0;
	card combined[maxCardsAmount];

	for (uint8_t i = 0; i < g->stats.playerHandCounter; i++) {
		combined[totalPlayerCards++] = g->player.hand[i];
	}
	for (uint8_t i = 0; i < g->stats.playerFaceUpCounter; i++) {
		combined[totalPlayerCards++] = g->player.faceUpCards[i];
	}
	for (uint8_t i = 0; i < g->stats.playerFaceDownCounter; i++) {
		combined[totalPlayerCards++] = g->player.faceDownCards[i];
	}
	(void)allPlayerCards;

	return forcePickupFromKnown(hand, handCount, combined, totalPlayerCards, top, s);
}

cardSelection aiCheaterSelectPlay(const game* g, gameSettings s, const card* cards, uint8_t count, card top) {
	cardSelection sel = tryCompleteFourOfAKind(g, cards, count, top, s);
	if (!selectionIsEmpty(sel)) return sel;

	sel = forcePickupFromPlayerDirect((card*)cards, count, g, top, s);
	if (!selectionIsEmpty(sel)) return sel;

	sel = findLowestStandardValidSingle(cards, count, top, s);
	if (!selectionIsEmpty(sel)) return sel;

	return findMagicCard(cards, count, top, s, cheaterMagicPriority, 3);
}
