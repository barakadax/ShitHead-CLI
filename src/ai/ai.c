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
#include <string.h>

typedef cardSelection (*selectPlayFn)(const game* g, cardSet hand, playContext ctx);

static selectPlayFn selectPlayForDifficulty(uint8_t difficulty) {
	switch ((aiDifficultyKind)difficulty) {
	case difficultyEasy:    return aiEasySelectPlay;
	case difficultyMedium:  return aiMediumSelectPlay;
	case difficultyHard:    return aiHardSelectPlay;
	case difficultyCheater: return aiCheaterSelectPlay;
	default:                return aiEasySelectPlay;
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
	playContext ctx = {topEffectiveCard(g), s};
	selectPlayFn selectPlay = selectPlayForDifficulty(s.aiDifficulty);
	cardSet hand = {g->ai.hand, g->stats.aiHandCounter};
	cardSelection sel = selectPlay(g, hand, ctx);

	if (selectionIsEmpty(sel)) {
		printf("AI picks up the pile.\n");
		pickupPileToAi(g);
		sortSingleHand(g->ai.hand, g->stats.aiHandCounter);
		return 0;
	}

	card played[maxCardsAmount];
	for (uint8_t i = 0; i < sel.count; i++) {
		played[i] = g->ai.hand[sel.indices[i]];
	}
	cardIndexList playedIdx = {.count = sel.count};
	memcpy(playedIdx.indices, sel.indices, sel.count);
	g->stats.aiHandCounter = removeCardIndices(g->ai.hand, g->stats.aiHandCounter, playedIdx);
	printPlayedCards(played, sel.count);
	drawUpToThreeForAi(g, s);
	pushCardsToPile(g, played, sel.count);
	return resolveAnotherTurn(g, played[0].value, s);
}

static uint8_t aiTakeTurnFromFaceUp(game* g, gameSettings s) {
	playContext ctx = {topEffectiveCard(g), s};
	selectPlayFn selectPlay = selectPlayForDifficulty(s.aiDifficulty);
	cardSet hand = {g->ai.faceUpCards, g->stats.aiFaceUpCounter};
	cardSelection sel = selectPlay(g, hand, ctx);

	if (selectionIsEmpty(sel)) {
		printf("AI picks up the pile.\n");
		pickupPileToAi(g);
		sortSingleHand(g->ai.hand, g->stats.aiHandCounter);
		return 0;
	}

	card played[defaultTableCardsAmount];
	for (uint8_t i = 0; i < sel.count; i++) {
		played[i] = g->ai.faceUpCards[sel.indices[i]];
	}
	cardIndexList playedIdx = {.count = sel.count};
	memcpy(playedIdx.indices, sel.indices, sel.count);
	g->stats.aiFaceUpCounter = removeCardIndices(g->ai.faceUpCards, g->stats.aiFaceUpCounter, playedIdx);
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
	playContext ctx = {topEffectiveCard(g), s};
	uint32_t randomIdx = randombytes_uniform(g->stats.aiFaceDownCounter);
	card chosen = g->ai.faceDownCards[randomIdx];

	g->stats.aiFaceDownCounter = removeCardIndices(g->ai.faceDownCards, g->stats.aiFaceDownCounter,
		(cardIndexList){.count = 1, .indices = {(uint8_t)randomIdx}});

	printf("AI plays face-down: ");
	printCard(chosen);
	printf("\n");

	if (!canPlayCardOn(chosen, ctx)) {
		printf("AI's face-down card is invalid! AI picks up.\n");
		g->ai.hand[g->stats.aiHandCounter++] = chosen;
		sortSingleHand(g->ai.hand, g->stats.aiHandCounter);
		pickupPileToAi(g);
		return 0;
	}

	pushCardsToPile(g, &chosen, 1);
	return resolveAnotherTurn(g, chosen.value, s);
}

uint8_t aiTakeTurn(game* g, gameSettings s) {
	if (g->stats.aiHandCounter > 0) return aiTakeTurnFromHand(g, s);
	if (g->stats.aiFaceUpCounter > 0) return aiTakeTurnFromFaceUp(g, s);
	return aiTakeTurnFromFaceDown(g, s);
}
