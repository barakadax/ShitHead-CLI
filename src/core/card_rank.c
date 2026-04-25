#include "card_rank.h"

static const uint8_t rankTable[14] = {
	0,
	13,
	1,
	2,
	3,
	4,
	5,
	6,
	7,
	8,
	9,
	10,
	11,
	12,
};

uint8_t cardRank(uint8_t value) {
	return rankTable[value];
}

uint8_t isNonStarterValue(uint8_t value) {
	return value == 1 || value == 2 || value == 3 || value == 7 || value == 8 || value == 10;
}
