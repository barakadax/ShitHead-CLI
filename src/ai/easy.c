#include "easy.h"
#include "ai_common.h"

static const uint8_t easyMagicPriority[] = {10, 2, 3};

cardSelection aiEasySelectPlay(const game* g, cardSet hand, playContext ctx) {
	(void)g;
	cardSelection sel = findHighestStandardValidSingle(hand, ctx);
	if (!selectionIsEmpty(sel)) return sel;
	return findMagicCard(hand, ctx, (valueList){easyMagicPriority, 3});
}
