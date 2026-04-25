#include "debug.h"
#include "ui/ui.h"
#include <stdio.h>

static void printDebugSettings(gameSettings s) {
	printf("Settings:\n");
	printf("  split deck: %s\n", s.splitDeck ? "ON" : "OFF");
	printf("  magic number seven: %s\n", s.magicNumberSeven ? "YES" : "NO");
	printf("  allow to put ten on seven: %s\n", s.tenOnSeven ? "YES" : "NO");
	printf("  player auto card sort: %s\n", s.playerAutoOrder ? "ON" : "OFF");
	printf("  hinting: %s\n", s.hinting ? "ON" : "OFF");
	printf("  AI difficulty: %s\n", s.aiDifficulty == difficultyEasy ? "EASY" :
	                                s.aiDifficulty == difficultyMedium ? "MEDIUM" :
	                                s.aiDifficulty == difficultyHard ? "HARD" : "CHEATER");
	printf("  magic number eight: %s\n", s.magicNumberEight ? "YES" : "NO");
	printf("  three on eight: %s\n", s.threeOnEight ? "YES" : "NO");
	printf("  allow voluntary pickup: %s\n", s.allowVoluntaryPickup ? "YES" : "NO");
	printf("  card sounds: %s\n", s.cardSounds ? "ON" : "OFF");
	printf("  music: %s\n", s.music ? "ON" : "OFF");
	printf("  auto save: %s\n", s.autoSave ? "ON" : "OFF");
}

static void printDebugStats(const game* g) {
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
}

static void printDebugCards(const game* g, gameSettings s) {
	printf("\nPlayer Hand: ");
	for (int i = 0; i < g->stats.playerHandCounter; i++) printCard(g->player.hand[i]);

	printf("\nPlayer Face Up: ");
	for (int i = 0; i < g->stats.playerFaceUpCounter; i++) printCard(g->player.faceUpCards[i]);

	printf("\nPlayer Face Down: ");
	for (int i = 0; i < g->stats.playerFaceDownCounter; i++) printCard(g->player.faceDownCards[i]);

	printf("\n\nAI Hand: ");
	for (int i = 0; i < g->stats.aiHandCounter; i++) printCard(g->ai.hand[i]);

	printf("\nAI Face Up: ");
	for (int i = 0; i < g->stats.aiFaceUpCounter; i++) printCard(g->ai.faceUpCards[i]);

	printf("\nAI Face Down: ");
	for (int i = 0; i < g->stats.aiFaceDownCounter; i++) printCard(g->ai.faceDownCards[i]);

	printf("\n\nCards the computer knows are in the player hand: ");
	for (int i = 0; i < g->stats.aiKnownPlayerCardsCounter; i++) printCard(g->aiKnownPlayerCards[i]);

	printf("\n\nRest of the pile: ");
	for (int i = 0; i < g->stats.deckCounter; i++) printCard(g->deck[i]);

	if (s.splitDeck) {
		printf("\n\nRest of the split deck: ");
		for (int i = 0; i < g->stats.splitDeckCounter; i++) printCard(g->splitDeck[i]);
	}
}

void printGame(const game* g, gameSettings settings) {
	printf("\n--- Game Debug State ---\n");
	printDebugSettings(settings);
	printDebugStats(g);
	printDebugCards(g, settings);
	printf("\n\nSize of game instance: %zu bytes\n", sizeof(*g));
	printf("Size of gameSettings instance: %zu bytes\n", sizeof(settings));
	printf("------------------------\n");
}
