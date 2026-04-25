#include "save.h"
#include "settings_loader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#  include <direct.h>
#  define mkdirPortable(p) _mkdir(p)
#else
#  include <sys/stat.h>
#  define mkdirPortable(p) mkdir((p), 0755)
#endif

static void slotDirectory(uint8_t slot, char* buf, size_t bufSize) {
	snprintf(buf, bufSize, "saves/slot%u", (unsigned)slot);
}

static void settingsPath(uint8_t slot, char* buf, size_t bufSize) {
	snprintf(buf, bufSize, "saves/slot%u/settings.json", (unsigned)slot);
}

static void statePath(uint8_t slot, char* buf, size_t bufSize) {
	snprintf(buf, bufSize, "saves/slot%u/state.json", (unsigned)slot);
}

static void ensureSlotDirectory(uint8_t slot) {
	mkdirPortable("saves");
	char dir[32];
	slotDirectory(slot, dir, sizeof(dir));
	mkdirPortable(dir);
}

uint8_t slotExists(uint8_t slot) {
	char path[64];
	statePath(slot, path, sizeof(path));
	FILE* f = fopen(path, "r");
	if (!f) return 0;
	fclose(f);
	return 1;
}

uint8_t slotIsFinished(uint8_t slot) {
	char path[64];
	statePath(slot, path, sizeof(path));
	FILE* f = fopen(path, "r");
	if (!f) return 0;
	char line[256];
	uint8_t result = 0;
	while (fgets(line, sizeof(line), f)) {
		if (strstr(line, "\"gameOver\"")) {
			char* colon = strchr(line, ':');
			if (colon && atoi(colon + 1) != 0) result = 1;
			break;
		}
	}
	fclose(f);
	return result;
}

static void writeCardArray(FILE* f, const char* key, const card* cards, uint8_t count) {
	fprintf(f, "  \"%s\": \"", key);
	for (uint8_t i = 0; i < count; i++) {
		if (i > 0) fprintf(f, ",");
		fprintf(f, "%u:%u", (unsigned)cards[i].value, (unsigned)cards[i].type);
	}
	fprintf(f, "\"");
}

static void writeStateCounters(FILE* f, const game* g) {
	fprintf(f, "  \"whoseTurn\": %u,\n", (unsigned)g->stats.whoseTurn);
	fprintf(f, "  \"deckCounter\": %u,\n", (unsigned)g->stats.deckCounter);
	fprintf(f, "  \"splitDeckCounter\": %u,\n", (unsigned)g->stats.splitDeckCounter);
	fprintf(f, "  \"pileCounter\": %u,\n", (unsigned)g->stats.pileCounter);
	fprintf(f, "  \"unusableCounter\": %u,\n", (unsigned)g->stats.unusableCounter);
	fprintf(f, "  \"aiKnownPlayerCardsCounter\": %u,\n", (unsigned)g->stats.aiKnownPlayerCardsCounter);
	fprintf(f, "  \"aiHandCounter\": %u,\n", (unsigned)g->stats.aiHandCounter);
	fprintf(f, "  \"aiFaceUpCounter\": %u,\n", (unsigned)g->stats.aiFaceUpCounter);
	fprintf(f, "  \"aiFaceDownCounter\": %u,\n", (unsigned)g->stats.aiFaceDownCounter);
	fprintf(f, "  \"playerHandCounter\": %u,\n", (unsigned)g->stats.playerHandCounter);
	fprintf(f, "  \"playerFaceUpCounter\": %u,\n", (unsigned)g->stats.playerFaceUpCounter);
	fprintf(f, "  \"playerFaceDownCounter\": %u,\n", (unsigned)g->stats.playerFaceDownCounter);
}

static void writeStateCardArrays(FILE* f, const game* g) {
	writeCardArray(f, "deck", g->deck, g->stats.deckCounter);
	fprintf(f, ",\n");
	writeCardArray(f, "splitDeck", g->splitDeck, g->stats.splitDeckCounter);
	fprintf(f, ",\n");
	writeCardArray(f, "pile", g->pile, g->stats.pileCounter);
	fprintf(f, ",\n");
	writeCardArray(f, "unusableCards", g->unusableCards, g->stats.unusableCounter);
	fprintf(f, ",\n");
	writeCardArray(f, "aiKnownPlayerCards", g->aiKnownPlayerCards, g->stats.aiKnownPlayerCardsCounter);
	fprintf(f, ",\n");
	writeCardArray(f, "playerHand", g->player.hand, g->stats.playerHandCounter);
	fprintf(f, ",\n");
	writeCardArray(f, "playerFaceUp", g->player.faceUpCards, g->stats.playerFaceUpCounter);
	fprintf(f, ",\n");
	writeCardArray(f, "playerFaceDown", g->player.faceDownCards, g->stats.playerFaceDownCounter);
	fprintf(f, ",\n");
	writeCardArray(f, "aiHand", g->ai.hand, g->stats.aiHandCounter);
	fprintf(f, ",\n");
	writeCardArray(f, "aiFaceUp", g->ai.faceUpCards, g->stats.aiFaceUpCounter);
	fprintf(f, ",\n");
	writeCardArray(f, "aiFaceDown", g->ai.faceDownCards, g->stats.aiFaceDownCounter);
	fprintf(f, "\n");
}

static int writeStateJson(const char* path, const game* g, uint8_t gameOver) {
	FILE* f = fopen(path, "w");
	if (!f) return -1;
	fprintf(f, "{\n");
	fprintf(f, "  \"gameOver\": %u,\n", (unsigned)gameOver);
	writeStateCounters(f, g);
	writeStateCardArrays(f, g);
	fprintf(f, "}\n");
	fclose(f);
	return 0;
}

int saveSlot(uint8_t slot, slotSnapshot snap) {
	ensureSlotDirectory(slot);
	char settPath[64];
	char stPath[64];
	settingsPath(slot, settPath, sizeof(settPath));
	statePath(slot, stPath, sizeof(stPath));
	if (saveSettings(&snap.settings, settPath) != 0) return -1;
	return writeStateJson(stPath, snap.g, snap.gameOver);
}

typedef enum {
	STATE_KEY_UNKNOWN,
	STATE_KEY_WHOSE_TURN,
	STATE_KEY_DECK_COUNTER,
	STATE_KEY_SPLIT_DECK_COUNTER,
	STATE_KEY_PILE_COUNTER,
	STATE_KEY_UNUSABLE_COUNTER,
	STATE_KEY_AI_KNOWN_COUNTER,
	STATE_KEY_AI_HAND_COUNTER,
	STATE_KEY_AI_FACE_UP_COUNTER,
	STATE_KEY_AI_FACE_DOWN_COUNTER,
	STATE_KEY_PLAYER_HAND_COUNTER,
	STATE_KEY_PLAYER_FACE_UP_COUNTER,
	STATE_KEY_PLAYER_FACE_DOWN_COUNTER,
	STATE_KEY_DECK,
	STATE_KEY_SPLIT_DECK,
	STATE_KEY_PILE,
	STATE_KEY_UNUSABLE,
	STATE_KEY_AI_KNOWN,
	STATE_KEY_PLAYER_HAND,
	STATE_KEY_PLAYER_FACE_UP,
	STATE_KEY_PLAYER_FACE_DOWN,
	STATE_KEY_AI_HAND,
	STATE_KEY_AI_FACE_UP,
	STATE_KEY_AI_FACE_DOWN,
} StateKey;

static StateKey getStateKey(const char* key) {
	if (strcmp(key, "whoseTurn") == 0) return STATE_KEY_WHOSE_TURN;
	if (strcmp(key, "deckCounter") == 0) return STATE_KEY_DECK_COUNTER;
	if (strcmp(key, "splitDeckCounter") == 0) return STATE_KEY_SPLIT_DECK_COUNTER;
	if (strcmp(key, "pileCounter") == 0) return STATE_KEY_PILE_COUNTER;
	if (strcmp(key, "unusableCounter") == 0) return STATE_KEY_UNUSABLE_COUNTER;
	if (strcmp(key, "aiKnownPlayerCardsCounter") == 0) return STATE_KEY_AI_KNOWN_COUNTER;
	if (strcmp(key, "aiHandCounter") == 0) return STATE_KEY_AI_HAND_COUNTER;
	if (strcmp(key, "aiFaceUpCounter") == 0) return STATE_KEY_AI_FACE_UP_COUNTER;
	if (strcmp(key, "aiFaceDownCounter") == 0) return STATE_KEY_AI_FACE_DOWN_COUNTER;
	if (strcmp(key, "playerHandCounter") == 0) return STATE_KEY_PLAYER_HAND_COUNTER;
	if (strcmp(key, "playerFaceUpCounter") == 0) return STATE_KEY_PLAYER_FACE_UP_COUNTER;
	if (strcmp(key, "playerFaceDownCounter") == 0) return STATE_KEY_PLAYER_FACE_DOWN_COUNTER;
	if (strcmp(key, "deck") == 0) return STATE_KEY_DECK;
	if (strcmp(key, "splitDeck") == 0) return STATE_KEY_SPLIT_DECK;
	if (strcmp(key, "pile") == 0) return STATE_KEY_PILE;
	if (strcmp(key, "unusableCards") == 0) return STATE_KEY_UNUSABLE;
	if (strcmp(key, "aiKnownPlayerCards") == 0) return STATE_KEY_AI_KNOWN;
	if (strcmp(key, "playerHand") == 0) return STATE_KEY_PLAYER_HAND;
	if (strcmp(key, "playerFaceUp") == 0) return STATE_KEY_PLAYER_FACE_UP;
	if (strcmp(key, "playerFaceDown") == 0) return STATE_KEY_PLAYER_FACE_DOWN;
	if (strcmp(key, "aiHand") == 0) return STATE_KEY_AI_HAND;
	if (strcmp(key, "aiFaceUp") == 0) return STATE_KEY_AI_FACE_UP;
	if (strcmp(key, "aiFaceDown") == 0) return STATE_KEY_AI_FACE_DOWN;
	return STATE_KEY_UNKNOWN;
}

static uint8_t parseCardArray(const char* str, card* out, uint8_t maxCount) {
	uint8_t count = 0;
	char buf[512];
	strncpy(buf, str, sizeof(buf) - 1);
	buf[sizeof(buf) - 1] = '\0';
	char* tok = strtok(buf, ",");
	while (tok && count < maxCount) {
		char* colon = strchr(tok, ':');
		if (colon) {
			*colon = '\0';
			out[count].value = (uint8_t)atoi(tok);
			out[count].type = (uint8_t)atoi(colon + 1);
			count++;
		}
		tok = strtok(NULL, ",");
	}
	return count;
}

static void applyStateCounter(game* g, StateKey sk, int val) {
	switch (sk) {
	case STATE_KEY_WHOSE_TURN:           g->stats.whoseTurn = val; break;
	case STATE_KEY_DECK_COUNTER:         g->stats.deckCounter = val; break;
	case STATE_KEY_SPLIT_DECK_COUNTER:   g->stats.splitDeckCounter = val; break;
	case STATE_KEY_PILE_COUNTER:         g->stats.pileCounter = val; break;
	case STATE_KEY_UNUSABLE_COUNTER:     g->stats.unusableCounter = val; break;
	case STATE_KEY_AI_KNOWN_COUNTER:     g->stats.aiKnownPlayerCardsCounter = val; break;
	case STATE_KEY_AI_HAND_COUNTER:      g->stats.aiHandCounter = val; break;
	case STATE_KEY_AI_FACE_UP_COUNTER:   g->stats.aiFaceUpCounter = val; break;
	case STATE_KEY_AI_FACE_DOWN_COUNTER: g->stats.aiFaceDownCounter = val; break;
	case STATE_KEY_PLAYER_HAND_COUNTER:  g->stats.playerHandCounter = val; break;
	case STATE_KEY_PLAYER_FACE_UP_COUNTER:   g->stats.playerFaceUpCounter = val; break;
	case STATE_KEY_PLAYER_FACE_DOWN_COUNTER: g->stats.playerFaceDownCounter = val; break;
	default: break;
	}
}

static void applyStateCardArray(game* g, StateKey sk, const char* arrStr) {
	switch (sk) {
	case STATE_KEY_DECK:         parseCardArray(arrStr, g->deck, maxCardsAmount); break;
	case STATE_KEY_SPLIT_DECK:   parseCardArray(arrStr, g->splitDeck, maxCardsAmount); break;
	case STATE_KEY_PILE:         parseCardArray(arrStr, g->pile, maxCardsAmount); break;
	case STATE_KEY_UNUSABLE:     parseCardArray(arrStr, g->unusableCards, maxCardsAmount); break;
	case STATE_KEY_AI_KNOWN:     parseCardArray(arrStr, g->aiKnownPlayerCards, maxCardsAmount); break;
	case STATE_KEY_PLAYER_HAND:  parseCardArray(arrStr, g->player.hand, maxCardsAmount); break;
	case STATE_KEY_PLAYER_FACE_UP:   parseCardArray(arrStr, g->player.faceUpCards, defaultTableCardsAmount); break;
	case STATE_KEY_PLAYER_FACE_DOWN: parseCardArray(arrStr, g->player.faceDownCards, defaultTableCardsAmount); break;
	case STATE_KEY_AI_HAND:      parseCardArray(arrStr, g->ai.hand, maxCardsAmount); break;
	case STATE_KEY_AI_FACE_UP:   parseCardArray(arrStr, g->ai.faceUpCards, defaultTableCardsAmount); break;
	case STATE_KEY_AI_FACE_DOWN: parseCardArray(arrStr, g->ai.faceDownCards, defaultTableCardsAmount); break;
	default: break;
	}
}

static int readStateJson(const char* path, game* g) {
	FILE* f = fopen(path, "r");
	if (!f) return -1;
	char line[1024];
	while (fgets(line, sizeof(line), f)) {
		char* keyStart = strchr(line, '"');
		if (!keyStart) continue;
		keyStart++;
		char* keyEnd = strchr(keyStart, '"');
		if (!keyEnd) continue;
		*keyEnd = '\0';
		char key[64];
		strncpy(key, keyStart, sizeof(key) - 1);
		key[sizeof(key) - 1] = '\0';

		char* colon = strchr(keyEnd + 1, ':');
		if (!colon) continue;
		char* valStart = colon + 1;
		while (*valStart == ' ') valStart++;

		StateKey sk = getStateKey(key);
		if (sk == STATE_KEY_UNKNOWN) continue;

		if (sk >= STATE_KEY_DECK) {
			char* arrStart = strchr(valStart, '"');
			if (!arrStart) continue;
			arrStart++;
			char* arrEnd = strchr(arrStart, '"');
			if (!arrEnd) continue;
			*arrEnd = '\0';
			applyStateCardArray(g, sk, arrStart);
		} else {
			applyStateCounter(g, sk, atoi(valStart));
		}
	}
	fclose(f);
	return 0;
}

int loadSlot(uint8_t slot, game* g, gameSettings* settings) {
	char settPath[64];
	char stPath[64];
	settingsPath(slot, settPath, sizeof(settPath));
	statePath(slot, stPath, sizeof(stPath));
	if (loadSettings(settings, settPath) != 0) return -1;
	return readStateJson(stPath, g);
}
