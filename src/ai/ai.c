#include "ai.h"
#include "core/deck.h"
#include "core/pile.h"
#include "core/rules.h"
#include "game/turn.h"
#include "ui/ui.h"
#include "cheater.h"
#include "easy.h"
#include "hard.h"
#include "medium.h"
#include <sodium.h>
#include <stdio.h>

typedef cardSelection (*selectPlayFn)(const game*, gameSettings, const card*, uint8_t, card);

static selectPlayFn selectPlayForDifficulty(uint8_t difficulty) {
	switch (difficulty) {
	case 0: return aiEasySelectPlay;
	case 1: return aiMediumSelectPlay;
	case 2: return aiHardSelectPlay;
	case 3: return aiCheaterSelectPlay;
	default: return aiEasySelectPlay;
	}
}

static void printPlayedCards(const card* cards, uint8_t count) {
	printf("AI plays:");
	for (uint8_t i = 0; i < count; i++) {
		printf(" ");
		printCard(cards[i]);
	}
	printf("\n");
}

static uint8_t aiTakeTurnFromHand(game* g, gameSettings s) {
	card top = topEffectiveCard(g);
	selectPlayFn selectPlay = selectPlayForDifficulty(s.aiDifficulty);
	cardSelection sel = selectPlay(g, s, g->ai.hand, g->stats.aiHandCounter, top);

	if (selectionIsEmpty(sel)) {
		printf("AI picks up the pile.\n");
		pickupPileIntoHand(g, 1);
		sortSingleHand(g->ai.hand, g->stats.aiHandCounter);
		return 0;
	}

	card played[maxCardsAmount];
	for (uint8_t i = 0; i < sel.count; i++) {
		played[i] = g->ai.hand[sel.indices[i]];
	}
	g->stats.aiHandCounter = removeCardIndicesFromHand(g->ai.hand, g->stats.aiHandCounter, sel.indices, sel.count);
	printPlayedCards(played, sel.count);
	drawUpToThreeForAi(g, s);
	pushCardsToPile(g, played, sel.count);
	return resolveAnotherTurn(g, played[0].value, s);
}

static uint8_t aiTakeTurnFromFaceUp(game* g, gameSettings s) {
	card top = topEffectiveCard(g);
	selectPlayFn selectPlay = selectPlayForDifficulty(s.aiDifficulty);
	cardSelection sel = selectPlay(g, s, g->ai.faceUpCards, g->stats.aiFaceUpCounter, top);

	if (selectionIsEmpty(sel)) {
		printf("AI picks up the pile.\n");
		pickupPileIntoHand(g, 1);
		sortSingleHand(g->ai.hand, g->stats.aiHandCounter);
		return 0;
	}

	card played[defaultTableCardsAmount];
	for (uint8_t i = 0; i < sel.count; i++) {
		played[i] = g->ai.faceUpCards[sel.indices[i]];
	}
	g->stats.aiFaceUpCounter = removeCardIndicesFromHand(g->ai.faceUpCards, g->stats.aiFaceUpCounter, sel.indices, sel.count);
	printf("AI plays face-up:");
	for (uint8_t i = 0; i < sel.count; i++) {
		printf(" ");
		printCard(played[i]);
	}
	printf("\n");
	pushCardsToPile(g, played, sel.count);
	return resolveAnotherTurn(g, played[0].value, s);
}

static uint8_t aiTakeTurnFromFaceDown(game* g, gameSettings s) {
	card top = topEffectiveCard(g);
	uint32_t randomIdx = randombytes_uniform(g->stats.aiFaceDownCounter);
	card chosen = g->ai.faceDownCards[randomIdx];

	g->stats.aiFaceDownCounter = removeCardIndicesFromHand(g->ai.faceDownCards, g->stats.aiFaceDownCounter,
		(uint8_t[]){(uint8_t)randomIdx}, 1);

	printf("AI plays face-down: ");
	printCard(chosen);
	printf("\n");

	if (!canPlayCardOn(chosen, top, s)) {
		printf("AI's face-down card is invalid! AI picks up.\n");
		g->ai.hand[g->stats.aiHandCounter++] = chosen;
		sortSingleHand(g->ai.hand, g->stats.aiHandCounter);
		pickupPileIntoHand(g, 1);
		return 0;
	}

	pushCardsToPile(g, &chosen, 1);
	return resolveAnotherTurn(g, chosen.value, s);
}

uint8_t aiTakeTurn(game* g, gameSettings s) {
	if (g->stats.aiHandCounter > 0) {
		return aiTakeTurnFromHand(g, s);
	}
	if (g->stats.aiFaceUpCounter > 0) {
		return aiTakeTurnFromFaceUp(g, s);
	}
	return aiTakeTurnFromFaceDown(g, s);
}
