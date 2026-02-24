#include "settings_loader.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void trim(char* str) {
	char* end;
	while (isspace((unsigned char)*str)) {
		str++;
	}
	if (*str == 0) {
		return;
	}
	end = str + strlen(str) - 1;
	while (end > str && isspace((unsigned char)*end)) {
		end--;
	}
	end[1] = '\0';
}

static SettingKey getSettingKey(const char* key) {
	if (strcmp(key, "splitDeck") == 0) {
		return KEY_SPLIT_DECK;
	}
	if (strcmp(key, "magicNumberSeven") == 0) {
		return KEY_MAGIC_SEVEN;
	}
	if (strcmp(key, "tenOnSeven") == 0) {
		return KEY_TEN_ON_SEVEN;
	}
	if (strcmp(key, "playerAutoOrder") == 0) {
		return KEY_PLAYER_AUTO_ORDER;
	}
	if (strcmp(key, "hinting") == 0) {
		return KEY_HINTING;
	}
	if (strcmp(key, "aiDifficulty") == 0) {
		return KEY_AI_DIFFICULTY;
	}
	if (strcmp(key, "magicNumberEight") == 0) {
		return KEY_MAGIC_EIGHT;
	}
	if (strcmp(key, "threeOnEight") == 0) {
		return KEY_THREE_ON_EIGHT;
	}
	if (strcmp(key, "allowVoluntaryPickup") == 0) {
		return KEY_ALLOW_VOLUNTARY_PICKUP;
	}
	if (strcmp(key, "cardSounds") == 0) {
		return KEY_CARD_SOUNDS;
	}
	if (strcmp(key, "music") == 0) {
		return KEY_MUSIC;
	}
	return KEY_UNKNOWN;
}

int loadSettings(gameSettings* settings, const char* filename) {
	FILE* file = fopen(filename, "r");
	if (!file) {
		return -1;
	}

	char line[256];
	while (fgets(line, sizeof(line), file)) {
		char* key = strtok(line, ":");
		char* val = strtok(NULL, ",}\n");

		if (key && val) {
			char* k_start = strchr(key, '"');
			if (k_start) {
				k_start++;
				char* k_end = strchr(k_start, '"');
				if (k_end) {
					*k_end = '\0';
				}
				key = k_start;
			}
			trim(key);
			trim(val);

			int value = atoi(val);

			switch (getSettingKey(key)) {
			case KEY_SPLIT_DECK:
				settings->splitDeck = value;
				break;
			case KEY_MAGIC_SEVEN:
				settings->magicNumberSeven = value;
				break;
			case KEY_TEN_ON_SEVEN:
				settings->tenOnSeven = value;
				break;
			case KEY_PLAYER_AUTO_ORDER:
				settings->playerAutoOrder = value;
				break;
			case KEY_HINTING:
				settings->hinting = value;
				break;
			case KEY_AI_DIFFICULTY:
				settings->aiDifficulty = value;
				break;
			case KEY_MAGIC_EIGHT:
				settings->magicNumberEight = value;
				break;
			case KEY_THREE_ON_EIGHT:
				settings->threeOnEight = value;
				break;
			case KEY_ALLOW_VOLUNTARY_PICKUP:
				settings->allowVoluntaryPickup = value;
				break;
			case KEY_CARD_SOUNDS:
				settings->cardSounds = value;
				break;
			case KEY_MUSIC:
				settings->music = value;
				break;
			default:
				printf("Unknown setting: %s\n", key);
				break;
			}
		}
	}

	fclose(file);
	return 0;
}
