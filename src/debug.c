#include "debug.h"
#include <stdio.h>

void printCard(card c) {
	if (c.value == 0) {
		return;
	}

	const char* suits[] = {"♥", "♦", "♠", "♣"};
    const char* ranks[] = {"", "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"};

    printf("[%s:%s]", ranks[c.value], suits[c.type % 4]);
}

void printCardsArray(const char* name, const card* cards, int amount) {
	printf("%s: ", name);
	for (int i = 0; i < amount; i++) {
		printCard(cards[i]);
		if (i < amount - 1) {
			printf(", ");
		}
	}
	printf("\n");
}

void printGame(const game* g, gameSettings settings) {
	printf("\n--- Game Debug State ---\n");
	printf("Settings:\n");
	printf("  split deck: %s\n", settings.splitDeck ? "ON" : "OFF");
	printf("  magic number seven: %s\n", settings.magicNumberSeven ? "YES" : "NO");
	printf("  allow to put ten on seven: %s\n", settings.tenOnSeven ? "YES" : "NO");
	printf("  player auto card sort: %s\n", settings.playerAutoOrder ? "ON" : "OFF");
	printf("  hinting: %s\n", settings.hinting ? "ON" : "OFF");
	printf("  AI difficulty: %s\n", settings.aiDifficulty == 0 ? "EASY" :
									settings.aiDifficulty == 1 ? "MEDIUM" :
									settings.aiDifficulty == 2 ? "HARD" :
																 "CHEATER");
	printf("  magic number eight: %s\n", settings.magicNumberEight ? "YES" : "NO");
	printf("  three on eight: %s\n", settings.threeOnEight ? "YES" : "NO");
	printf("  allow voluntary pickup: %s\n", settings.allowVoluntaryPickup ? "YES" : "NO");
	printf("  card sounds: %s\n", settings.cardSounds ? "ON" : "OFF");
	printf("  music: %s\n", settings.music ? "ON" : "OFF");
	printf("  auto save: %s\n", settings.autoSave ? "ON" : "OFF");

	printf("\nStats:\n");
	printf("  whoseTurn: %s\n", g->stats.whoseTurn ? "Computer" : "Player");
	printf("  deckCounter: %d\n", g->stats.deckCounter);
	printf("  splitDeckCounter: %d\n", g->stats.splitDeckCounter);
	printf("  pileCounter: %d\n", g->stats.pileCounter);
	printf("  unusableCounter: %d\n", g->stats.unusableCounter);
	printf("  aiKnownPlayerCardsCounter: %d\n", g->stats.aiKnownPlayerCardsCounter);
	printf("  aiHandCounter: %d\n", g->stats.aiHandCounter);
	printf("  aiFaceUpCounter: %d\n", g->stats.aiFaceUpCounter);
	printf("  aiFaceDownCounter: %d\n", g->stats.aiFaceDownCounter);
	printf("  playerHandCounter: %d\n", g->stats.playerHandCounter);
	printf("  playerFaceUpCounter: %d\n", g->stats.playerFaceUpCounter);
	printf("  playerFaceDownCounter: %d\n", g->stats.playerFaceDownCounter);

	printf("\nPlayer Hand: ");
	for (int i = 0; i < g->stats.playerHandCounter; i++) {
		printCard(g->player.hand[i]);
	}

	printf("\nPlayer Face Up: ");
	for (int i = 0; i < g->stats.playerFaceUpCounter; i++) {
		printCard(g->player.faceUpCards[i]);
	}

	printf("\nPlayer Face Down: ");
	for (int i = 0; i < g->stats.playerFaceDownCounter; i++) {
		printCard(g->player.faceDownCards[i]);
	}

	printf("\n\nAI Hand: ");
	for (int i = 0; i < g->stats.aiHandCounter; i++) {
		printCard(g->ai.hand[i]);
	}

	printf("\nAI Face Up: ");
	for (int i = 0; i < g->stats.aiFaceUpCounter; i++) {
		printCard(g->ai.faceUpCards[i]);
	}

	printf("\nAI Face Down: ");
	for (int i = 0; i < g->stats.aiFaceDownCounter; i++) {
		printCard(g->ai.faceDownCards[i]);
	}

	printf("\n\nCards the computer knows are in the player hand: ");
	for (int i = 0; i < g->stats.aiKnownPlayerCardsCounter; i++) {
		printCard(g->aiKnownPlayerCards[i]);
	}

	printf("\n\nRest of the pile: ");
	for (int i = 0; i < g->stats.deckCounter; i++) {
		printCard(g->deck[i]);
	}

	if (settings.splitDeck) {
		printf("\n\nRest of the split deck: ");
		for (int i = 0; i < g->stats.splitDeckCounter; i++) {
			printCard(g->splitDeck[i]);
		}
	}

	printf("\n\nSize of game instance: %zu bytes\n", sizeof(*g));
	printf("Size of gameSettings instance: %zu bytes\n", sizeof(settings));
	printf("------------------------\n");
}
