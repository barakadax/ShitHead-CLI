#include "hint.h"
#include "ui.h"
#include <stdio.h>

void printHints(const card* hand, uint8_t handCount, card top, gameSettings s) {
	uint8_t indices[maxCardsAmount];
	uint8_t count = 0;
	legalCardIndicesFromHand(hand, handCount, top, s, indices, &count);
	if (count == 0) {
		printf("(no legal plays — must pick up)\n");
		return;
	}
	printf("Cards you can play hint:");
	for (uint8_t i = 0; i < count; i++) {
		printf(" %u:", (unsigned)(indices[i] + 1));
		extern void printCard(card c);
		printCard(hand[indices[i]]);
	}
	printf("\n");
}
