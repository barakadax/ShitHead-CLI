#include "rules.h"

uint8_t canPlayCardOn(card c, card top, gameSettings s) {
	if (top.value == 0) {
		return 1;
	}
	if (c.value == 2) {
		return 1;
	}
	if (c.value == 3) {
		return 1;
	}
	if (c.value == 10) {
		if (s.magicNumberSeven && top.value == 7 && !s.tenOnSeven) {
			return 0;
		}
		return 1;
	}
	if (top.value == 1) {
		return c.value == 1;
	}
	if (s.magicNumberSeven && top.value == 7) {
		return cardRank(c.value) <= cardRank(7);
	}
	return cardRank(c.value) >= cardRank(top.value);
}

void legalCardIndicesFromHand(const card* hand, uint8_t handCount, card top, gameSettings s, uint8_t* outIndices, uint8_t* outCount) {
	*outCount = 0;
	for (uint8_t i = 0; i < handCount; i++) {
		if (canPlayCardOn(hand[i], top, s)) {
			outIndices[(*outCount)++] = i;
		}
	}
}

void legalCardIndicesFromFaceUp(const card* faceUp, uint8_t faceUpCount, card top, gameSettings s, uint8_t* outIndices, uint8_t* outCount) {
	*outCount = 0;
	for (uint8_t i = 0; i < faceUpCount; i++) {
		if (canPlayCardOn(faceUp[i], top, s)) {
			outIndices[(*outCount)++] = i;
		}
	}
}

uint8_t hasAnyLegalPlayInHand(const card* hand, uint8_t handCount, card top, gameSettings s) {
	for (uint8_t i = 0; i < handCount; i++) {
		if (canPlayCardOn(hand[i], top, s)) {
			return 1;
		}
	}
	return 0;
}
