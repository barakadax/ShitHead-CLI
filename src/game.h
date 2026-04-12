#ifndef GAME_H
#define GAME_H

#include <stdint.h>

#define maxCardsAmount 52
#define amountOfTypesOfCards 4
#define defaultTableCardsAmount 3
#define amountOfPlayers 2
#define initPlayerTableStates 3
#define minimumStarterValue 4

typedef struct card {
	uint8_t value : 4;
	uint8_t type : 2;
} card;

typedef struct gameStats {
	uint8_t whoseTurn : 1;
	uint8_t deckCounter : 6;
	uint8_t splitDeckCounter : 5;
	uint8_t pileCounter : 6;
	uint8_t unusableCounter : 6;
	uint8_t aiKnownPlayerCardsCounter : 6;
	uint8_t aiHandCounter : 6;
	uint8_t aiFaceUpCounter : 3;
	uint8_t aiFaceDownCounter : 3;
	uint8_t playerHandCounter : 6;
	uint8_t playerFaceUpCounter : 3;
	uint8_t playerFaceDownCounter : 3;
} gameStats;

typedef struct player {
	card hand[maxCardsAmount];
	card faceUpCards[defaultTableCardsAmount];
	card faceDownCards[defaultTableCardsAmount];
} player;

typedef struct game {
	gameStats stats;
	player ai;
	player player;
	card deck[maxCardsAmount];
	card splitDeck[maxCardsAmount];
	card pile[maxCardsAmount];
	card unusableCards[maxCardsAmount];
	card aiKnownPlayerCards[maxCardsAmount];
} game;

typedef enum {
	KEY_UNKNOWN,
	KEY_SPLIT_DECK,
	KEY_MAGIC_SEVEN,
	KEY_TEN_ON_SEVEN,
	KEY_PLAYER_AUTO_ORDER,
	KEY_HINTING,
	KEY_AI_DIFFICULTY,
	KEY_MAGIC_EIGHT,
	KEY_THREE_ON_EIGHT,
	KEY_ALLOW_VOLUNTARY_PICKUP,
	KEY_CARD_SOUNDS,
	KEY_MUSIC,
	KEY_AUTO_SAVE
} SettingKey;

typedef struct gameSettings {
	uint8_t splitDeck : 1;
	uint8_t magicNumberSeven : 1;
	uint8_t tenOnSeven : 1;
	uint8_t playerAutoOrder : 1;
	uint8_t hinting : 1;
	uint8_t aiDifficulty : 2; // 0: easy, 1: medium, 2: hard, 3: cheater
	uint8_t magicNumberEight : 1;
	uint8_t threeOnEight : 1;
	uint8_t allowVoluntaryPickup : 1;
	uint8_t cardSounds : 1;
	uint8_t music : 1;
	uint8_t autoSave : 1;
} gameSettings;

#endif // GAME_H
