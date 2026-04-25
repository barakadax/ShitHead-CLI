#include "turn.h"
#include "ai/ai.h"
#include "core/deck.h"
#include "ui/hint.h"
#include "core/pile.h"
#include "core/rules.h"
#include "io/save.h"
#include "ui/ui.h"
#include "ai/ai_common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef DEBUG
#include "debug/debug.h"
#endif

static uint8_t isGameOver(const game* g) {
	if (g->stats.playerHandCounter == 0 && g->stats.playerFaceUpCounter == 0 && g->stats.playerFaceDownCounter == 0) return 1;
	if (g->stats.aiHandCounter == 0 && g->stats.aiFaceUpCounter == 0 && g->stats.aiFaceDownCounter == 0) return 2;
	return 0;
}

uint8_t applyMagicEffects(game* g, uint8_t playedValue, gameSettings s) {
	const char* actor = g->stats.whoseTurn == 0 ? "You" : "AI";
	if (playedValue == 10) {
		burnPile(g);
		printf("All cards from the pile were sent into the shadow realm, %s get%s another turn.\n",
			actor, g->stats.whoseTurn == 0 ? "" : "s");
		return 1;
	}
	if (countConsecutiveTopMatches(g) >= 4) {
		burnPile(g);
		printf("All cards from the pile were sent into the shadow realm, %s get%s another turn.\n",
			actor, g->stats.whoseTurn == 0 ? "" : "s");
		return 1;
	}
	if (playedValue == 8 && s.magicNumberEight) return 1;
	return 0;
}

uint8_t applyThreeOnEightBonus(game* g, gameSettings s) {
	if (!s.threeOnEight) return 0;
	if (g->stats.pileCounter < 2) return 0;
	int16_t underneathIdx = (int16_t)g->stats.pileCounter - 2;
	while (underneathIdx >= 0 && g->pile[underneathIdx].value == 3) {
		underneathIdx--;
	}
	if (underneathIdx >= 0 && g->pile[underneathIdx].value == 8) return 1;
	return 0;
}

uint8_t resolveAnotherTurn(game* g, uint8_t playedValue, gameSettings s) {
	uint8_t anotherTurn = applyMagicEffects(g, playedValue, s);
	if (!anotherTurn && playedValue == 3) {
		anotherTurn = applyThreeOnEightBonus(g, s);
	}
	return anotherTurn;
}

static void removeFromKnownIfPresent(game* g, card played) {
	for (uint8_t i = 0; i < g->stats.aiKnownPlayerCardsCounter; i++) {
		if (g->aiKnownPlayerCards[i].value == played.value && g->aiKnownPlayerCards[i].type == played.type) {
			for (uint8_t j = i; j < g->stats.aiKnownPlayerCardsCounter - 1; j++) {
				g->aiKnownPlayerCards[j] = g->aiKnownPlayerCards[j + 1];
			}
			g->aiKnownPlayerCards[--g->stats.aiKnownPlayerCardsCounter] = (card){0, 0};
			return;
		}
	}
}

static uint8_t promptCardCount(const card* cards, uint8_t cardCount, uint8_t cardValue) {
	uint8_t available = 0;
	for (uint8_t i = 0; i < cardCount; i++) {
		if (cards[i].value == cardValue) available++;
	}
	if (available <= 1) return 1;
	printf("You have %u cards of that value. How many to play? (1-%u): ", (unsigned)available, (unsigned)available);
	fflush(stdout);
	char buf[8];
	if (!fgets(buf, sizeof(buf), stdin)) return 1;
	int n = atoi(buf);
	if (n < 1 || (uint8_t)n > available) return 1;
	return (uint8_t)n;
}

static uint8_t playerTurnFromHand(game* g, gameSettings s) {
	playContext ctx = {topEffectiveCard(g), s};
	printBoard(g);

	if (s.hinting) {
		printHints((cardSet){g->player.hand, g->stats.playerHandCounter}, ctx);
	}

	uint8_t canPickup = s.allowVoluntaryPickup && g->stats.pileCounter > 0;
	if (canPickup) {
		printf("Options: enter card number, 'p' to pick up pile, 'r' for rules");
	} else {
		printf("Options: enter card number, 'r' for rules");
	}
#ifdef DEBUG
	printf(", 'd' for debug");
#endif
	printf(": ");
	fflush(stdout);

	char buf[16];
	if (!fgets(buf, sizeof(buf), stdin)) return 0;

	if (buf[0] == 'r') {
		printRules();
		return playerTurnFromHand(g, s);
	}
#ifdef DEBUG
	if (buf[0] == 'd') {
		printGame(g, s);
		return playerTurnFromHand(g, s);
	}
#endif
	if (canPickup && buf[0] == 'p') {
		printf("You pick up the pile.\n");
		addPileToKnownPlayerCards(g);
		pickupPileToPlayer(g);
		if (s.playerAutoOrder) sortSingleHand(g->player.hand, g->stats.playerHandCounter);
		return 0;
	}

	int idx = atoi(buf) - 1;
	if (idx < 0 || (uint8_t)idx >= g->stats.playerHandCounter) {
		printf("Invalid choice.\n");
		return playerTurnFromHand(g, s);
	}

	card chosen = g->player.hand[idx];
	if (!canPlayCardOn(chosen, ctx)) {
		printf("That card cannot be played here.\n");
		return playerTurnFromHand(g, s);
	}

	uint8_t count = promptCardCount(g->player.hand, g->stats.playerHandCounter, chosen.value);
	card toPlay[maxCardsAmount];
	cardIndexList toRemove = {0};
	for (uint8_t i = 0; i < g->stats.playerHandCounter && toRemove.count < count; i++) {
		if (g->player.hand[i].value == chosen.value) {
			toPlay[toRemove.count] = g->player.hand[i];
			toRemove.indices[toRemove.count++] = i;
		}
	}

	for (uint8_t i = 0; i < toRemove.count; i++) {
		removeFromKnownIfPresent(g, toPlay[i]);
	}

	uint8_t totalToPlay = toRemove.count;
	if (toRemove.count == g->stats.playerHandCounter) {
		cardIndexList faceUpToRemove = {0};
		for (uint8_t i = 0; i < g->stats.playerFaceUpCounter; i++) {
			if (g->player.faceUpCards[i].value == chosen.value) {
				toPlay[totalToPlay++] = g->player.faceUpCards[i];
				faceUpToRemove.indices[faceUpToRemove.count++] = i;
			}
		}
		if (faceUpToRemove.count > 0) {
			g->stats.playerFaceUpCounter = removeCardIndices(g->player.faceUpCards, g->stats.playerFaceUpCounter, faceUpToRemove);
		}
	}

	g->stats.playerHandCounter = removeCardIndices(g->player.hand, g->stats.playerHandCounter, toRemove);
	drawUpToThreeForPlayer(g, s);
	pushCardsToPile(g, toPlay, totalToPlay);

	return resolveAnotherTurn(g, chosen.value, s);
}

static uint8_t playerTurnFromFaceUp(game* g, gameSettings s) {
	playContext ctx = {topEffectiveCard(g), s};
	printBoard(g);

	if (s.hinting) {
		printHints((cardSet){g->player.faceUpCards, g->stats.playerFaceUpCounter}, ctx);
	}

	uint8_t canPickup = s.allowVoluntaryPickup && g->stats.pileCounter > 0;
	printf("Enter face-up card number to play");
	if (canPickup) printf(", 'p' to pick up pile");
	printf(", 'r' for rules");
#ifdef DEBUG
	printf(", 'd' for debug");
#endif
	printf(": ");
	fflush(stdout);

	char buf[16];
	if (!fgets(buf, sizeof(buf), stdin)) return 0;

	if (buf[0] == 'r') {
		printRules();
		return playerTurnFromFaceUp(g, s);
	}
#ifdef DEBUG
	if (buf[0] == 'd') {
		printGame(g, s);
		return playerTurnFromFaceUp(g, s);
	}
#endif
	if (canPickup && buf[0] == 'p') {
		printf("You pick up the pile.\n");
		addPileToKnownPlayerCards(g);
		pickupPileToPlayer(g);
		if (s.playerAutoOrder) sortSingleHand(g->player.hand, g->stats.playerHandCounter);
		return 0;
	}

	int idx = atoi(buf) - 1;
	if (idx < 0 || (uint8_t)idx >= g->stats.playerFaceUpCounter) {
		printf("Invalid choice.\n");
		return playerTurnFromFaceUp(g, s);
	}

	card chosen = g->player.faceUpCards[idx];
	if (!canPlayCardOn(chosen, ctx)) {
		printf("That card cannot be played here.\n");
		return playerTurnFromFaceUp(g, s);
	}

	uint8_t count = promptCardCount(g->player.faceUpCards, g->stats.playerFaceUpCounter, chosen.value);
	card toPlay[defaultTableCardsAmount];
	cardIndexList toRemove = {0};
	for (uint8_t i = 0; i < g->stats.playerFaceUpCounter && toRemove.count < count; i++) {
		if (g->player.faceUpCards[i].value == chosen.value) {
			toPlay[toRemove.count] = g->player.faceUpCards[i];
			toRemove.indices[toRemove.count++] = i;
		}
	}

	g->stats.playerFaceUpCounter = removeCardIndices(g->player.faceUpCards, g->stats.playerFaceUpCounter, toRemove);
	pushCardsToPile(g, toPlay, toRemove.count);

	return resolveAnotherTurn(g, chosen.value, s);
}

static uint8_t playerTurnFromFaceDown(game* g, gameSettings s) {
	playContext ctx = {topEffectiveCard(g), s};
	printBoard(g);

	printf("You have %u face-down card%s. Enter number (1-%u) to play blind, 'r' for rules",
		(unsigned)g->stats.playerFaceDownCounter,
		g->stats.playerFaceDownCounter == 1 ? "" : "s",
		(unsigned)g->stats.playerFaceDownCounter);
#ifdef DEBUG
	printf(", 'd' for debug");
#endif
	printf(": ");
	fflush(stdout);

	char buf[16];
	if (!fgets(buf, sizeof(buf), stdin)) return 0;

	if (buf[0] == 'r') {
		printRules();
		return playerTurnFromFaceDown(g, s);
	}
#ifdef DEBUG
	if (buf[0] == 'd') {
		printGame(g, s);
		return playerTurnFromFaceDown(g, s);
	}
#endif

	int idx = atoi(buf) - 1;
	if (idx < 0 || (uint8_t)idx >= g->stats.playerFaceDownCounter) {
		printf("Invalid choice.\n");
		return playerTurnFromFaceDown(g, s);
	}

	card chosen = g->player.faceDownCards[idx];
	printf("Revealed: ");
	printCard(chosen);
	printf("\n");

	g->stats.playerFaceDownCounter = removeCardIndices(g->player.faceDownCards, g->stats.playerFaceDownCounter,
		(cardIndexList){.count = 1, .indices = {(uint8_t)idx}});

	if (!canPlayCardOn(chosen, ctx)) {
		printf("Invalid — card goes to your hand and you pick up the pile.\n");
		g->player.hand[g->stats.playerHandCounter++] = chosen;
		if (s.playerAutoOrder) sortSingleHand(g->player.hand, g->stats.playerHandCounter);
		addPileToKnownPlayerCards(g);
		pickupPileToPlayer(g);
		if (s.playerAutoOrder) sortSingleHand(g->player.hand, g->stats.playerHandCounter);
		return 0;
	}

	pushCardsToPile(g, &chosen, 1);

	return resolveAnotherTurn(g, chosen.value, s);
}

static uint8_t playerTakeTurn(game* g, gameSettings s) {
	if (g->stats.playerHandCounter > 0) return playerTurnFromHand(g, s);
	if (g->stats.playerFaceUpCounter > 0) return playerTurnFromFaceUp(g, s);
	return playerTurnFromFaceDown(g, s);
}

uint8_t runGameLoop(game* g, gameSettings s, uint8_t slot) {
	while (1) {
		uint8_t over = isGameOver(g);
		if (over) {
			if (over == 1) {
				printf("\n*** You win! The AI is the ShitHead! ***\n");
			} else {
				printf("\n*** You lose! You are the ShitHead! ***\n");
			}
			saveSlot(slot, (slotSnapshot){g, s, 1});
			return over;
		}

		if (g->stats.whoseTurn == 0) {
			if (s.autoSave) {
				saveSlot(slot, (slotSnapshot){g, s, 0});
			}
			uint8_t anotherTurn = playerTakeTurn(g, s);
			if (!anotherTurn) g->stats.whoseTurn = 1;
		} else {
			printf("\n--- AI's turn ---\n");
			uint8_t anotherTurn = aiTakeTurn(g, s);
			if (!anotherTurn) g->stats.whoseTurn = 0;
		}
	}
}
