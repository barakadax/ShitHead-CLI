#include "hint.h"
#include "ui.h"
#include <stdio.h>

void printHints(cardSet hand, playContext ctx) {
	cardIndexList legal = legalCardIndices(hand, ctx);
	if (legal.count == 0) {
		printf("(no legal plays — must pick up)\n");
		return;
	}
	printf("Cards you can play hint:");
	for (uint8_t i = 0; i < legal.count; i++) {
		printf(" %u:", (unsigned)(legal.indices[i] + 1));
		printCard(hand.cards[legal.indices[i]]);
	}
	printf("\n");
}
