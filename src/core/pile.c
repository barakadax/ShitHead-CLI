#include "pile.h"
#include <string.h>

void pushCardsToPile(game* g, const card* cards, uint8_t count) {
	for (uint8_t i = 0; i < count; i++) {
		g->pile[g->stats.pileCounter++] = cards[i];
	}
}

void burnPile(game* g) {
	for (uint8_t i = 0; i < g->stats.pileCounter; i++) {
		g->unusableCards[g->stats.unusableCounter++] = g->pile[i];
		g->pile[i] = (card){0, 0};
	}
	g->stats.pileCounter = 0;
}

void pickupPileToPlayer(game* g) {
	for (uint8_t i = 0; i < g->stats.pileCounter; i++) {
		g->player.hand[g->stats.playerHandCounter++] = g->pile[i];
		g->pile[i] = (card){0, 0};
	}
	g->stats.pileCounter = 0;
}

void pickupPileToAi(game* g) {
	for (uint8_t i = 0; i < g->stats.pileCounter; i++) {
		g->ai.hand[g->stats.aiHandCounter++] = g->pile[i];
		g->pile[i] = (card){0, 0};
	}
	g->stats.pileCounter = 0;
}

void addPileToKnownPlayerCards(game* g) {
	for (uint8_t i = 0; i < g->stats.pileCounter; i++) {
		g->aiKnownPlayerCards[g->stats.aiKnownPlayerCardsCounter++] = g->pile[i];
	}
}

card topEffectiveCard(const game* g) {
	card empty = {0, 0};
	if (g->stats.pileCounter == 0) return empty;
	int16_t i = (int16_t)g->stats.pileCounter - 1;
	while (i >= 0 && g->pile[i].value == 3) {
		i--;
	}
	if (i < 0) return empty;
	return g->pile[i];
}

uint8_t countConsecutiveTopMatches(const game* g) {
	if (g->stats.pileCounter == 0) return 0;
	uint8_t topValue = g->pile[g->stats.pileCounter - 1].value;
	uint8_t count = 0;
	int16_t i = (int16_t)g->stats.pileCounter - 1;
	while (i >= 0 && g->pile[i].value == topValue) {
		count++;
		i--;
	}
	return count;
}
