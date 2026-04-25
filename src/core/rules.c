#include "rules.h"

uint8_t canPlayCardOn(card c, playContext ctx) {
	if (ctx.top.value == 0) return 1;
	if (c.value == 2) return 1;
	if (c.value == 3) return 1;
	if (c.value == 10) {
		if (ctx.settings.magicNumberSeven && ctx.top.value == 7 && !ctx.settings.tenOnSeven) return 0;
		return 1;
	}
	if (ctx.top.value == 1) return c.value == 1;
	if (ctx.settings.magicNumberSeven && ctx.top.value == 7) return cardRank(c.value) <= cardRank(7);
	return cardRank(c.value) >= cardRank(ctx.top.value);
}

cardIndexList legalCardIndices(cardSet cards, playContext ctx) {
	cardIndexList result = {0};
	for (uint8_t i = 0; i < cards.count; i++) {
		if (canPlayCardOn(cards.cards[i], ctx)) {
			result.indices[result.count++] = i;
		}
	}
	return result;
}

uint8_t hasAnyLegalPlay(cardSet cards, playContext ctx) {
	for (uint8_t i = 0; i < cards.count; i++) {
		if (canPlayCardOn(cards.cards[i], ctx)) return 1;
	}
	return 0;
}
