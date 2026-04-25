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

static uint8_t isGameOver(const game* g) {
	if (g->stats.playerHandCounter == 0 && g->stats.playerFaceUpCounter == 0 && g->stats.playerFaceDownCounter == 0) {
		return 1;
	}
	if (g->stats.aiHandCounter == 0 && g->stats.aiFaceUpCounter == 0 && g->stats.aiFaceDownCounter == 0) {
		return 2;
	}
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
	if (playedValue == 8 && s.magicNumberEight) {
		return 1;
	}
	return 0;
}

uint8_t applyThreeOnEightBonus(game* g, gameSettings s) {
	if (!s.threeOnEight) return 0;
	if (g->stats.pileCounter < 2) return 0;
	int16_t underneathIdx = (int16_t)g->stats.pileCounter - 2;
	while (underneathIdx >= 0 && g->pile[underneathIdx].value == 3) {
		underneathIdx--;
	}
	if (underneathIdx >= 0 && g->pile[underneathIdx].value == 8) {
		return 1;
	}
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
	card top = topEffectiveCard(g);
	printBoard(g, s);

	if (s.hinting) {
		printHints(g->player.hand, g->stats.playerHandCounter, top, s);
	}

	if (s.allowVoluntaryPickup) {
		printf("Options: enter card number, or 'p' to pick up pile: ");
	} else {
		printf("Enter card number to play: ");
	}
	fflush(stdout);

	char buf[16];
	if (!fgets(buf, sizeof(buf), stdin)) return 0;

	if (s.allowVoluntaryPickup && buf[0] == 'p') {
		printf("You pick up the pile.\n");
		addPileToKnownPlayerCards(g);
		pickupPileIntoHand(g, 0);
		if (s.playerAutoOrder) sortSingleHand(g->player.hand, g->stats.playerHandCounter);
		return 0;
	}

	int idx = atoi(buf) - 1;
	if (idx < 0 || (uint8_t)idx >= g->stats.playerHandCounter) {
		printf("Invalid choice.\n");
		return playerTurnFromHand(g, s);
	}

	card chosen = g->player.hand[idx];
	if (!canPlayCardOn(chosen, top, s)) {
		printf("That card cannot be played here.\n");
		return playerTurnFromHand(g, s);
	}

	uint8_t count = promptCardCount(g->player.hand, g->stats.playerHandCounter, chosen.value);
	card toPlay[maxCardsAmount];
	uint8_t playIndices[maxCardsAmount];
	uint8_t found = 0;
	for (uint8_t i = 0; i < g->stats.playerHandCounter && found < count; i++) {
		if (g->player.hand[i].value == chosen.value) {
			toPlay[found] = g->player.hand[i];
			playIndices[found] = i;
			found++;
		}
	}

	for (uint8_t i = 0; i < found; i++) {
		removeFromKnownIfPresent(g, toPlay[i]);
	}

	g->stats.playerHandCounter = removeCardIndicesFromHand(g->player.hand, g->stats.playerHandCounter, playIndices, found);
	drawUpToThreeForPlayer(g, s);
	pushCardsToPile(g, toPlay, found);

	return resolveAnotherTurn(g, chosen.value, s);
}

static uint8_t playerTurnFromFaceUp(game* g, gameSettings s) {
	card top = topEffectiveCard(g);
	printBoard(g, s);

	if (s.hinting) {
		printHints(g->player.faceUpCards, g->stats.playerFaceUpCounter, top, s);
	}

	printf("Enter face-up card number to play");
	if (s.allowVoluntaryPickup) printf(", or 'p' to pick up pile");
	printf(": ");
	fflush(stdout);

	char buf[16];
	if (!fgets(buf, sizeof(buf), stdin)) return 0;

	if (s.allowVoluntaryPickup && buf[0] == 'p') {
		printf("You pick up the pile.\n");
		addPileToKnownPlayerCards(g);
		pickupPileIntoHand(g, 0);
		if (s.playerAutoOrder) sortSingleHand(g->player.hand, g->stats.playerHandCounter);
		return 0;
	}

	int idx = atoi(buf) - 1;
	if (idx < 0 || (uint8_t)idx >= g->stats.playerFaceUpCounter) {
		printf("Invalid choice.\n");
		return playerTurnFromFaceUp(g, s);
	}

	card chosen = g->player.faceUpCards[idx];
	if (!canPlayCardOn(chosen, top, s)) {
		printf("That card cannot be played here.\n");
		return playerTurnFromFaceUp(g, s);
	}

	uint8_t count = promptCardCount(g->player.faceUpCards, g->stats.playerFaceUpCounter, chosen.value);
	card toPlay[defaultTableCardsAmount];
	uint8_t playIndices[defaultTableCardsAmount];
	uint8_t found = 0;
	for (uint8_t i = 0; i < g->stats.playerFaceUpCounter && found < count; i++) {
		if (g->player.faceUpCards[i].value == chosen.value) {
			toPlay[found] = g->player.faceUpCards[i];
			playIndices[found] = i;
			found++;
		}
	}

	g->stats.playerFaceUpCounter = removeCardIndicesFromHand(g->player.faceUpCards, g->stats.playerFaceUpCounter, playIndices, found);
	pushCardsToPile(g, toPlay, found);

	return resolveAnotherTurn(g, chosen.value, s);
}

static uint8_t playerTurnFromFaceDown(game* g, gameSettings s) {
	card top = topEffectiveCard(g);
	printBoard(g, s);

	printf("You have %u face-down card%s. Enter number (1-%u) to play blind: ",
		(unsigned)g->stats.playerFaceDownCounter,
		g->stats.playerFaceDownCounter == 1 ? "" : "s",
		(unsigned)g->stats.playerFaceDownCounter);
	fflush(stdout);

	char buf[16];
	if (!fgets(buf, sizeof(buf), stdin)) return 0;

	int idx = atoi(buf) - 1;
	if (idx < 0 || (uint8_t)idx >= g->stats.playerFaceDownCounter) {
		printf("Invalid choice.\n");
		return playerTurnFromFaceDown(g, s);
	}

	card chosen = g->player.faceDownCards[idx];
	printf("Revealed: ");
	printCard(chosen);
	printf("\n");

	g->stats.playerFaceDownCounter = removeCardIndicesFromHand(g->player.faceDownCards, g->stats.playerFaceDownCounter, (uint8_t[]){idx}, 1);

	if (!canPlayCardOn(chosen, top, s)) {
		printf("Invalid — card goes to your hand and you pick up the pile.\n");
		g->player.hand[g->stats.playerHandCounter++] = chosen;
		if (s.playerAutoOrder) sortSingleHand(g->player.hand, g->stats.playerHandCounter);
		addPileToKnownPlayerCards(g);
		pickupPileIntoHand(g, 0);
		if (s.playerAutoOrder) sortSingleHand(g->player.hand, g->stats.playerHandCounter);
		return 0;
	}

	pushCardsToPile(g, &chosen, 1);

	return resolveAnotherTurn(g, chosen.value, s);
}

static uint8_t playerTakeTurn(game* g, gameSettings s) {
	if (g->stats.playerHandCounter > 0) {
		return playerTurnFromHand(g, s);
	}
	if (g->stats.playerFaceUpCounter > 0) {
		return playerTurnFromFaceUp(g, s);
	}
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
			saveSlot(slot, g, s, 1);
			return over;
		}

		if (g->stats.whoseTurn == 0) {
			if (s.autoSave) {
				saveSlot(slot, g, s, 0);
			}
			uint8_t anotherTurn = playerTakeTurn(g, s);
			if (!anotherTurn) {
				g->stats.whoseTurn = 1;
			}
		} else {
			printf("\n--- AI's turn ---\n");
			uint8_t anotherTurn = aiTakeTurn(g, s);
			if (!anotherTurn) {
				g->stats.whoseTurn = 0;
			}
		}
	}
}
