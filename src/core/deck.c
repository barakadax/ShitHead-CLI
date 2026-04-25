#include "deck.h"
#include <sodium.h>
#include <string.h>

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

void sortSingleHand(card* hand, uint8_t count) {
	qsort(hand, count, sizeof(card), compareCards);
}

void drawUpToThreeForPlayer(game* g, gameSettings settings) {
	while (g->stats.playerHandCounter < defaultTableCardsAmount && g->stats.deckCounter > 0) {
		g->player.hand[g->stats.playerHandCounter] = g->deck[g->stats.deckCounter - 1];
		g->deck[g->stats.deckCounter - 1] = (card){0, 0};
		g->stats.playerHandCounter++;
		g->stats.deckCounter--;
	}
	if (settings.playerAutoOrder) {
		sortSingleHand(g->player.hand, g->stats.playerHandCounter);
	}
}

void drawUpToThreeForAi(game* g, gameSettings settings) {
	if (settings.splitDeck) {
		while (g->stats.aiHandCounter < defaultTableCardsAmount && g->stats.splitDeckCounter > 0) {
			g->ai.hand[g->stats.aiHandCounter] = g->splitDeck[g->stats.splitDeckCounter - 1];
			g->splitDeck[g->stats.splitDeckCounter - 1] = (card){0, 0};
			g->stats.aiHandCounter++;
			g->stats.splitDeckCounter--;
		}
	} else {
		while (g->stats.aiHandCounter < defaultTableCardsAmount && g->stats.deckCounter > 0) {
			g->ai.hand[g->stats.aiHandCounter] = g->deck[g->stats.deckCounter - 1];
			g->deck[g->stats.deckCounter - 1] = (card){0, 0};
			g->stats.aiHandCounter++;
			g->stats.deckCounter--;
		}
	}
	sortSingleHand(g->ai.hand, g->stats.aiHandCounter);
}
