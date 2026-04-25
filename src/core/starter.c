#include "starter.h"
#include "card_rank.h"

static uint8_t lowestNonStarterRankInHand(const card* hand, uint8_t count) {
	uint8_t lowest = 255;
	for (uint8_t i = 0; i < count; i++) {
		if (!isNonStarterValue(hand[i].value)) {
			uint8_t r = cardRank(hand[i].value);
			if (r < lowest) {
				lowest = r;
			}
		}
	}
	return lowest;
}

void determineWhoStarts(game* g) {
	uint8_t playerLowest = lowestNonStarterRankInHand(g->player.hand, g->stats.playerHandCounter);
	uint8_t aiLowest = lowestNonStarterRankInHand(g->ai.hand, g->stats.aiHandCounter);

	if (playerLowest == 255 && aiLowest == 255) {
		g->stats.whoseTurn = 0;
		return;
	}
	if (playerLowest == 255) {
		g->stats.whoseTurn = 1;
		return;
	}
	if (aiLowest == 255) {
		g->stats.whoseTurn = 0;
		return;
	}
	g->stats.whoseTurn = playerLowest > aiLowest ? 1 : 0;
}
