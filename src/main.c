#include "difficulties/easy.h"
#include "game.h"
#include "settings_loader.h"
#include <sodium.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef DEBUG
#include "debug.h"
#endif

void initGame(game* g, gameSettings settings) {
	memset(g, 0, sizeof(game));

	uint8_t count = 0;
	for (int8_t type = amountOfTypesOfCards - 1; type >= 0; type--) {
		for (int8_t val = maxCardsAmount / amountOfTypesOfCards; val >= 1; val--) {
			g->deck[count].type = type;
			g->deck[count].value = val;
			count++;
		}
	}

	for (int8_t i = maxCardsAmount - 1; i > 0; i--) {
		uint32_t j = randombytes_uniform(i + 1);
		card temp = g->deck[i];
		g->deck[i] = g->deck[j];
		g->deck[j] = temp;
	}

	if (settings.splitDeck) {
		for (int8_t i = 0; i < maxCardsAmount / 2; i++) {
			g->splitDeck[i] = g->deck[i + maxCardsAmount / 2];
			g->deck[i + maxCardsAmount / 2].value = 0;
			g->deck[i + maxCardsAmount / 2].type = 0;
		}
		g->stats.deckCounter = maxCardsAmount / 2;
		g->stats.splitDeckCounter = maxCardsAmount / 2;
	} else {
		g->stats.deckCounter = maxCardsAmount;
		g->stats.splitDeckCounter = 0;
	}
}

void initHandCards(game* g, gameSettings settings) {
	uint8_t playerHandCounter = 0;
	uint8_t playerFaceUpCounter = 0;
	uint8_t playerFaceDownCounter = 0;
	uint8_t aiHandCounter = 0;
	uint8_t aiFaceUpCounter = 0;
	uint8_t aiFaceDownCounter = 0;
	uint8_t mod = 0;
	card currentCard;

	for (int8_t i = defaultTableCardsAmount * amountOfPlayers * initPlayerTableStates; i > 0; i--) {
		mod = i % (defaultTableCardsAmount + initPlayerTableStates);
		if (settings.splitDeck && (mod == 3 || mod == 4 || mod == 5)) {
			currentCard = g->splitDeck[g->stats.splitDeckCounter - 1];
			g->splitDeck[g->stats.splitDeckCounter - 1].value = 0;
			g->splitDeck[g->stats.splitDeckCounter - 1].type = 0;
			g->stats.splitDeckCounter--;
		} else {
			currentCard = g->deck[g->stats.deckCounter - 1];
			g->deck[g->stats.deckCounter - 1].value = 0;
			g->deck[g->stats.deckCounter - 1].type = 0;
			g->stats.deckCounter--;
		}

		switch (mod) {
		case 0:
			g->player.hand[playerHandCounter++] = currentCard;
			g->stats.playerHandCounter++;
			break;
		case 1:
			g->player.faceUpCards[playerFaceUpCounter++] = currentCard;
			g->stats.playerFaceUpCounter++;
			break;
		case 2:
			g->player.faceDownCards[playerFaceDownCounter++] = currentCard;
			g->stats.playerFaceDownCounter++;
			break;
		case 3:
			g->ai.hand[aiHandCounter++] = currentCard;
			g->stats.aiHandCounter++;
			break;
		case 4:
			g->ai.faceUpCards[aiFaceUpCounter++] = currentCard;
			g->stats.aiFaceUpCounter++;
			break;
		case 5:
			g->ai.faceDownCards[aiFaceDownCounter++] = currentCard;
			g->stats.aiFaceDownCounter++;
			break;
		}
	}
}

void compactDeck(card* deck) {
	uint8_t writeIdx = 0;
	for (uint8_t readIdx = 0; readIdx < maxCardsAmount; readIdx++) {
		if (deck[readIdx].value != 0) {
			if (writeIdx != readIdx) {
				deck[writeIdx] = deck[readIdx];
				deck[readIdx].value = 0;
				deck[readIdx].type = 0;
			}
			writeIdx++;
		}
	}
}

void determineWhoStarts(game* g) { // 0 is player, 1 is ai
	uint8_t playerMin = g->player.hand[0].value;
	uint8_t aiMin = g->ai.hand[0].value;

	if (playerMin < minimumStarterValue) {
		g->stats.whoseTurn = 1;
	} else if (aiMin < minimumStarterValue) {
		g->stats.whoseTurn = 0;
	} else {
		for (uint8_t i = 1; i < g->stats.playerHandCounter; i++) {
			if (g->player.hand[i].value >= minimumStarterValue && g->player.hand[i].value < playerMin) {
				playerMin = g->player.hand[i].value;
			}
		}

		for (uint8_t i = 1; i < g->stats.aiHandCounter; i++) {
			if (g->ai.hand[i].value >= minimumStarterValue && g->ai.hand[i].value < aiMin) {
				aiMin = g->ai.hand[i].value;
			}
		}

		g->stats.whoseTurn = playerMin > aiMin;
	}
}

int compareCards(const void* a, const void* b) {
	const card* cardA = (const card*)a;
	const card* cardB = (const card*)b;
	return (int)cardB->value - (int)cardA->value;
}

void sortHands(game* g, gameSettings settings, uint8_t forcePlayerSort) {
	if (forcePlayerSort || settings.playerAutoOrder) {
		qsort(g->player.hand, g->stats.playerHandCounter, sizeof(card), compareCards);
	}
	qsort(g->ai.hand, g->stats.aiHandCounter, sizeof(card), compareCards);
}

void main() {
	if (sodium_init() < 0) {
		printf("Sodium initialization failed\n");
		return;
	}

	game g;
	gameSettings settings = {0};
	if (loadSettings(&settings, "settings.json") != 0) {
		settings = (gameSettings) {.splitDeck = 1,
								   .magicNumberSeven = 1,
								   .tenOnSeven = 0,
								   .playerAutoOrder = 1,
								   .hinting = 0,
								   .aiDifficulty = 3,
								   .magicNumberEight = 1,
								   .threeOnEight = 1,
								   .allowVoluntaryPickup = 1,
								   .cardSounds = 0,
								   .music = 0};
		printf("Settings file not found, using default settings\n");
	}

	initGame(&g, settings);
	initHandCards(&g, settings);
	sortHands(&g, settings, 1);
	compactDeck(g.deck);
	if (settings.splitDeck) {
		compactDeck(g.splitDeck);
	}
	determineWhoStarts(&g);

#ifdef DEBUG
	printGame(&g, settings);
#endif

	a(&g, settings);
}