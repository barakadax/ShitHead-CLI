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

cardSelection findHighestStandardValidSingle(const card* hand, uint8_t handCount, card top, gameSettings s) {
	cardSelection best = emptySelection();
	uint8_t bestRank = 0;
	for (uint8_t i = 0; i < handCount; i++) {
		uint8_t v = hand[i].value;
		if (v == 2 || v == 3 || v == 7 || v == 8 || v == 10 || v == 1) continue;
		if (!canPlayCardOn(hand[i], top, s)) continue;
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

cardSelection findLowestStandardValidSingle(const card* hand, uint8_t handCount, card top, gameSettings s) {
	cardSelection best = emptySelection();
	uint8_t lowestRank = 255;
	for (uint8_t i = 0; i < handCount; i++) {
		uint8_t v = hand[i].value;
		if (v == 2 || v == 3 || v == 7 || v == 8 || v == 10 || v == 1) continue;
		if (!canPlayCardOn(hand[i], top, s)) continue;
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

cardSelection findLowestValidSetMaxQuantity(const card* hand, uint8_t handCount, card top, gameSettings s) {
	cardSelection best = emptySelection();
	uint8_t lowestRank = 255;

	for (uint8_t i = 0; i < handCount; i++) {
		if (!canPlayCardOn(hand[i], top, s)) continue;
		uint8_t r = cardRank(hand[i].value);
		if (r < lowestRank) {
			lowestRank = r;
		}
	}
	if (lowestRank == 255) return best;

	for (uint8_t i = 0; i < handCount; i++) {
		if (cardRank(hand[i].value) == lowestRank && canPlayCardOn(hand[i], top, s)) {
			best.indices[best.count++] = i;
			best.cardValue = hand[i].value;
		}
	}
	return best;
}

cardSelection findMagicCard(const card* hand, uint8_t handCount, card top, gameSettings s, const uint8_t* priorityValues, uint8_t priorityCount) {
	for (uint8_t p = 0; p < priorityCount; p++) {
		uint8_t targetValue = priorityValues[p];
		for (uint8_t i = 0; i < handCount; i++) {
			if (hand[i].value == targetValue && canPlayCardOn(hand[i], top, s)) {
				cardSelection sel = emptySelection();
				sel.cardValue = targetValue;
				sel.indices[sel.count++] = i;
				return sel;
			}
		}
	}
	return emptySelection();
}

cardSelection tryCompleteFourOfAKind(const game* g, const card* hand, uint8_t handCount, card top, gameSettings s) {
	if (g->stats.pileCounter == 0) return emptySelection();
	uint8_t topValue = g->pile[g->stats.pileCounter - 1].value;
	uint8_t consecutiveOnPile = 0;
	for (int16_t i = (int16_t)g->stats.pileCounter - 1; i >= 0 && g->pile[i].value == topValue; i--) {
		consecutiveOnPile++;
	}
	uint8_t neededToComplete = (uint8_t)(4 - consecutiveOnPile);
	if (neededToComplete == 0 || neededToComplete > handCount) return emptySelection();

	cardSelection sel = emptySelection();
	for (uint8_t i = 0; i < handCount && sel.count < neededToComplete; i++) {
		if (hand[i].value == topValue && canPlayCardOn(hand[i], top, s)) {
			sel.indices[sel.count++] = i;
			sel.cardValue = topValue;
		}
	}
	if (sel.count < neededToComplete) return emptySelection();
	return sel;
}

cardSelection forcePickupFromKnown(const card* hand, uint8_t handCount, const card* knownPlayerCards,
	uint8_t knownCount, card top, gameSettings s) {
	if (knownCount == 0) return emptySelection();

	uint8_t playerHighestRank = 0;
	for (uint8_t i = 0; i < knownCount; i++) {
		uint8_t r = cardRank(knownPlayerCards[i].value);
		if (r > playerHighestRank) {
			playerHighestRank = r;
		}
	}

	cardSelection best = emptySelection();
	uint8_t bestRank = 0;
	for (uint8_t i = 0; i < handCount; i++) {
		uint8_t v = hand[i].value;
		if (v == 2 || v == 3 || v == 10) continue;
		if (!canPlayCardOn(hand[i], top, s)) continue;
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

uint8_t removeCardIndicesFromHand(card* hand, uint8_t handCount, const uint8_t* indices, uint8_t indexCount) {
	uint8_t removeFlags[maxCardsAmount] = {0};
	for (uint8_t i = 0; i < indexCount; i++) {
		removeFlags[indices[i]] = 1;
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
