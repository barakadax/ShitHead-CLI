#include "ui.h"
#include "core/pile.h"
#include <stdio.h>

static const char* suitSymbols[] = {"♥", "♦", "♠", "♣"};
static const char* rankNames[] = {"", "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"};

void printCard(card c) {
	if (c.value == 0) {
		return;
	}
	printf("[%s%s]", rankNames[c.value], suitSymbols[c.type % 4]);
}

static void printCardRow(const card* cards, uint8_t count) {
	for (uint8_t i = 0; i < count; i++) {
		printf(" %u:", (unsigned)(i + 1));
		printCard(cards[i]);
	}
}

void printPlayerHand(const game* g) {
	printf("Your hand:");
	printCardRow(g->player.hand, g->stats.playerHandCounter);
	printf("\n");
}

void printPlayerFaceUp(const game* g) {
	printf("Your face-up:");
	printCardRow(g->player.faceUpCards, g->stats.playerFaceUpCounter);
	printf("\n");
}

static void printAiFaceUp(const game* g) {
	printf("AI face-up:");
	for (uint8_t i = 0; i < g->stats.aiFaceUpCounter; i++) {
		printf(" ");
		printCard(g->ai.faceUpCards[i]);
	}
	printf("\n");
}

static void printPileTop(const game* g) {
	card top = topEffectiveCard(g);
	printf("Pile top: ");
	if (top.value == 0) {
		printf("[empty]");
	} else {
		printCard(top);
		printf(" (%u card%s)", (unsigned)g->stats.pileCounter, g->stats.pileCounter == 1 ? "" : "s");
	}
	printf("\n");
}

void printBoard(const game* g, gameSettings s) {
	(void)s;
	printf("\n---\n");
	printf("AI: %u hand | %u face-up | %u face-down\n", (unsigned)g->stats.aiHandCounter,
		(unsigned)g->stats.aiFaceUpCounter, (unsigned)g->stats.aiFaceDownCounter);
	printAiFaceUp(g);
	printPileTop(g);
	if (g->stats.playerFaceUpCounter > 0) {
		printPlayerFaceUp(g);
	}
	if (g->stats.playerHandCounter > 0) {
		printPlayerHand(g);
	} else if (g->stats.playerFaceUpCounter == 0) {
		printf("Your face-down: %u card%s remaining\n", (unsigned)g->stats.playerFaceDownCounter,
			g->stats.playerFaceDownCounter == 1 ? "" : "s");
	}
}

void promptEnterToContinue(void) {
	printf("Press Enter to continue...");
	fflush(stdout);
	int c;
	while ((c = getchar()) != '\n' && c != EOF) {
	}
}
