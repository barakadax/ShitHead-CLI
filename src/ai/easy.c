#include "easy.h"
#include "ai_common.h"
#include "core/pile.h"
#include "core/deck.h"

static const uint8_t easyMagicPriority[] = {10, 2, 3};

cardSelection aiEasySelectPlay(const game* g, gameSettings s, const card* cards, uint8_t count, card top) {
	(void)g;
	cardSelection sel = findHighestStandardValidSingle(cards, count, top, s);
	if (!selectionIsEmpty(sel)) return sel;
	return findMagicCard(cards, count, top, s, easyMagicPriority, 3);
}
