#include "swap.h"
#include "core/card_rank.h"
#include "core/deck.h"
#include "ui/ui.h"
#include <stdio.h>
#include <string.h>

static uint8_t swapPriorityRank(uint8_t value) {
	if (value == 2) return 100;
	if (value == 3) return 99;
	if (value == 10) return 98;
	return cardRank(value);
}

static void swapCards(card* a, card* b) {
	card tmp = *a;
	*a = *b;
	*b = tmp;
}

static void aiAutoSwap(game* g, gameSettings settings) {
	for (uint8_t faceUpIdx = 0; faceUpIdx < g->stats.aiFaceUpCounter; faceUpIdx++) {
		uint8_t worstFaceUpPrio = swapPriorityRank(g->ai.faceUpCards[faceUpIdx].value);
		int8_t bestHandIdx = -1;
		uint8_t bestHandPrio = worstFaceUpPrio;

		for (uint8_t handIdx = 0; handIdx < g->stats.aiHandCounter; handIdx++) {
			uint8_t hp = swapPriorityRank(g->ai.hand[handIdx].value);
			if (hp > bestHandPrio) {
				bestHandPrio = hp;
				bestHandIdx = handIdx;
			}
		}

		if (bestHandIdx >= 0) {
			swapCards(&g->ai.faceUpCards[faceUpIdx], &g->ai.hand[bestHandIdx]);
		}
	}
	sortSingleHand(g->ai.hand, g->stats.aiHandCounter);
}

static void playerSwapPhase(game* g, gameSettings settings) {
	char input[16];
	while (1) {
		printf("\n--- Pre-game Swap ---\n");
		printf("Your hand:");
		for (uint8_t i = 0; i < g->stats.playerHandCounter; i++) {
			printf(" H%u:", (unsigned)(i + 1));
			printCard(g->player.hand[i]);
		}
		printf("\nYour face-up:");
		for (uint8_t i = 0; i < g->stats.playerFaceUpCounter; i++) {
			printf(" F%u:", (unsigned)(i + 1));
			printCard(g->player.faceUpCards[i]);
		}
		printf("\nEnter H<n> F<m> to swap (e.g. H2 F1), or 'done': ");
		fflush(stdout);

		if (!fgets(input, sizeof(input), stdin)) break;

		if (strncmp(input, "done", 4) == 0 || input[0] == 'd') break;

		unsigned handIdx = 0, faceIdx = 0;
		if (sscanf(input, "H%u F%u", &handIdx, &faceIdx) != 2) {
			printf("Invalid input. Use H<n> F<m> format.\n");
			continue;
		}
		handIdx--;
		faceIdx--;
		if (handIdx >= g->stats.playerHandCounter || faceIdx >= g->stats.playerFaceUpCounter) {
			printf("Index out of range.\n");
			continue;
		}
		swapCards(&g->player.hand[handIdx], &g->player.faceUpCards[faceIdx]);
		if (settings.playerAutoOrder) {
			sortSingleHand(g->player.hand, g->stats.playerHandCounter);
		}
	}
}

void runPreGameSwap(game* g, gameSettings settings) {
	playerSwapPhase(g, settings);
	aiAutoSwap(g, settings);
}
