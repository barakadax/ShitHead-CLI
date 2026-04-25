#include "ai_common.h"
#include "core/card_rank.h"
#include <string.h>

cardSelection emptySelection(void) {
	cardSelection sel;
	memset(&sel, 0, sizeof(sel));
	return sel;
}

uint8_t selectionIsEmpty(cardSelection sel) {
	return sel.count == 0;
}

cardSelection findHighestStandardValidSingle(cardSet hand, playContext ctx) {
	cardSelection best = emptySelection();
	uint8_t bestRank = 0;
	for (uint8_t i = 0; i < hand.count; i++) {
		uint8_t v = hand.cards[i].value;
		if (isNonStarterValue(v)) continue;
		if (!canPlayCardOn(hand.cards[i], ctx)) continue;
		uint8_t r = cardRank(v);
		if (r > bestRank) {
			bestRank = r;
			best.cardValue = v;
			best.count = 1;
			best.indices[0] = i;
		}
	}
	return best;
}

cardSelection findLowestStandardValidSingle(cardSet hand, playContext ctx) {
	cardSelection best = emptySelection();
	uint8_t lowestRank = 255;
	for (uint8_t i = 0; i < hand.count; i++) {
		uint8_t v = hand.cards[i].value;
		if (isNonStarterValue(v)) continue;
		if (!canPlayCardOn(hand.cards[i], ctx)) continue;
		uint8_t r = cardRank(v);
		if (r < lowestRank) {
			lowestRank = r;
			best.cardValue = v;
			best.count = 1;
			best.indices[0] = i;
		}
	}
	return best;
}

cardSelection findLowestValidSetMaxQuantity(cardSet hand, playContext ctx) {
	cardSelection best = emptySelection();
	uint8_t lowestRank = 255;
	for (uint8_t i = 0; i < hand.count; i++) {
		if (isNonStarterValue(hand.cards[i].value)) continue;
		if (!canPlayCardOn(hand.cards[i], ctx)) continue;
		uint8_t r = cardRank(hand.cards[i].value);
		if (r < lowestRank) lowestRank = r;
	}
	if (lowestRank == 255) return best;
	for (uint8_t i = 0; i < hand.count; i++) {
		if (isNonStarterValue(hand.cards[i].value)) continue;
		if (cardRank(hand.cards[i].value) == lowestRank && canPlayCardOn(hand.cards[i], ctx)) {
			best.indices[best.count++] = i;
			best.cardValue = hand.cards[i].value;
		}
	}
	return best;
}

cardSelection findMagicCard(cardSet hand, playContext ctx, valueList priority) {
	for (uint8_t p = 0; p < priority.count; p++) {
		uint8_t targetValue = priority.values[p];
		for (uint8_t i = 0; i < hand.count; i++) {
			if (hand.cards[i].value == targetValue && canPlayCardOn(hand.cards[i], ctx)) {
				cardSelection sel = emptySelection();
				sel.cardValue = targetValue;
				sel.indices[sel.count++] = i;
				return sel;
			}
		}
	}
	return emptySelection();
}

cardSelection tryCompleteFourOfAKind(const game* g, cardSet hand, playContext ctx) {
	if (g->stats.pileCounter == 0) return emptySelection();
	uint8_t topValue = g->pile[g->stats.pileCounter - 1].value;
	uint8_t consecutiveOnPile = 0;
	for (int16_t i = (int16_t)g->stats.pileCounter - 1; i >= 0 && g->pile[i].value == topValue; i--) {
		consecutiveOnPile++;
	}
	uint8_t neededToComplete = (uint8_t)(4 - consecutiveOnPile);
	if (neededToComplete == 0 || neededToComplete > hand.count) return emptySelection();
	cardSelection sel = emptySelection();
	for (uint8_t i = 0; i < hand.count && sel.count < neededToComplete; i++) {
		if (hand.cards[i].value == topValue && canPlayCardOn(hand.cards[i], ctx)) {
			sel.indices[sel.count++] = i;
			sel.cardValue = topValue;
		}
	}
	if (sel.count < neededToComplete) return emptySelection();
	return sel;
}

cardSelection forcePickupFromKnown(cardSet hand, cardSet known, playContext ctx) {
	if (known.count == 0) return emptySelection();
	uint8_t playerHighestRank = 0;
	for (uint8_t i = 0; i < known.count; i++) {
		uint8_t r = cardRank(known.cards[i].value);
		if (r > playerHighestRank) playerHighestRank = r;
	}
	cardSelection best = emptySelection();
	uint8_t bestRank = 0;
	for (uint8_t i = 0; i < hand.count; i++) {
		uint8_t v = hand.cards[i].value;
		if (v == 2 || v == 3 || v == 10) continue;
		if (!canPlayCardOn(hand.cards[i], ctx)) continue;
		uint8_t r = cardRank(v);
		if (r > playerHighestRank && r > bestRank) {
			bestRank = r;
			best.cardValue = v;
			best.count = 1;
			best.indices[0] = i;
		}
	}
	return best;
}

uint8_t removeCardIndices(card* hand, uint8_t handCount, cardIndexList indices) {
	uint8_t removeFlags[maxCardsAmount] = {0};
	for (uint8_t i = 0; i < indices.count; i++) {
		removeFlags[indices.indices[i]] = 1;
	}
	uint8_t writeIdx = 0;
	for (uint8_t readIdx = 0; readIdx < handCount; readIdx++) {
		if (!removeFlags[readIdx]) {
			hand[writeIdx++] = hand[readIdx];
		}
	}
	for (uint8_t i = writeIdx; i < handCount; i++) {
		hand[i] = (card){0, 0};
	}
	return writeIdx;
}
