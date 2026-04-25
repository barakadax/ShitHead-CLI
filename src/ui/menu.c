#include "menu.h"
#include "core/deck.h"
#include "io/save.h"
#include "io/settings_loader.h"
#include "core/starter.h"
#include "game/swap.h"
#include "game/turn.h"
#include "ui.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char* difficultyName(uint8_t d) {
	switch ((aiDifficultyKind)d) {
	case difficultyEasy:    return "Easy";
	case difficultyMedium:  return "Medium";
	case difficultyHard:    return "Hard";
	case difficultyCheater: return "Cheater";
	default:                return "?";
	}
}

static void printSettingsMenu(const gameSettings* s) {
	printf("\n=== Settings ===\n");
	printf(" 1. splitDeck            : %u\n", (unsigned)s->splitDeck);
	printf(" 2. magicNumberSeven     : %u\n", (unsigned)s->magicNumberSeven);
	printf(" 3. tenOnSeven           : %u\n", (unsigned)s->tenOnSeven);
	printf(" 4. playerAutoOrder      : %u\n", (unsigned)s->playerAutoOrder);
	printf(" 5. hinting              : %u\n", (unsigned)s->hinting);
	printf(" 6. aiDifficulty         : %u  (%s)\n", (unsigned)s->aiDifficulty, difficultyName(s->aiDifficulty));
	printf(" 7. magicNumberEight     : %u\n", (unsigned)s->magicNumberEight);
	printf(" 8. threeOnEight         : %u\n", (unsigned)s->threeOnEight);
	printf(" 9. allowVoluntaryPickup : %u\n", (unsigned)s->allowVoluntaryPickup);
	printf("10. cardSounds           : %u  (not implemented)\n", (unsigned)s->cardSounds);
	printf("11. music                : %u  (not implemented)\n", (unsigned)s->music);
	printf("12. autoSave             : %u\n", (unsigned)s->autoSave);
	printf(" s. Save and return\n");
	printf(" b. Back without saving\n");
	printf("Choice: ");
	fflush(stdout);
}

static void promptAiDifficulty(gameSettings* s) {
	printf("New aiDifficulty (0=Easy 1=Medium 2=Hard 3=Cheater): ");
	fflush(stdout);
	char buf[8];
	if (!fgets(buf, sizeof(buf), stdin)) return;
	int v = atoi(buf);
	if (v >= difficultyEasy && v <= difficultyCheater) {
		s->aiDifficulty = (uint8_t)v;
	} else {
		printf("Invalid value.\n");
	}
}

static void toggleSettingField(gameSettings* s, int choice) {
	switch (choice) {
	case 1:  s->splitDeck = !s->splitDeck; break;
	case 2:  s->magicNumberSeven = !s->magicNumberSeven; break;
	case 3:  s->tenOnSeven = !s->tenOnSeven; break;
	case 4:  s->playerAutoOrder = !s->playerAutoOrder; break;
	case 5:  s->hinting = !s->hinting; break;
	case 6:  promptAiDifficulty(s); break;
	case 7:  s->magicNumberEight = !s->magicNumberEight; break;
	case 8:  s->threeOnEight = !s->threeOnEight; break;
	case 9:  s->allowVoluntaryPickup = !s->allowVoluntaryPickup; break;
	case 10: s->cardSounds = !s->cardSounds; break;
	case 11: s->music = !s->music; break;
	case 12: s->autoSave = !s->autoSave; break;
	default: printf("Invalid choice.\n"); break;
	}
}

static void runSettingsMenu(void) {
	gameSettings s = {0};
	if (loadSettings(&s, "settings.json") != 0) {
		s = defaultGameSettings();
	}
	while (1) {
		printSettingsMenu(&s);
		char buf[16];
		if (!fgets(buf, sizeof(buf), stdin)) continue;
		if (buf[0] == 's') {
			saveSettings(&s, "settings.json");
			printf("Settings saved.\n");
			return;
		}
		if (buf[0] == 'b') return;
		toggleSettingField(&s, atoi(buf));
	}
}

static uint8_t promptSlot(void) {
	while (1) {
		printf("Choose slot (1-3): ");
		fflush(stdout);
		char buf[8];
		if (!fgets(buf, sizeof(buf), stdin)) continue;
		int n = atoi(buf);
		if (n >= 1 && n <= saveSlotCount) return (uint8_t)n;
		printf("Invalid slot.\n");
	}
}

static void startNewGame(void) {
	uint8_t slot = promptSlot();
	if (slotExists(slot)) {
		printf("Save slot %u exists. Overwriting...\n", (unsigned)slot);
	}

	game g;
	gameSettings settings = {0};
	if (loadSettings(&settings, "settings.json") != 0) {
		settings = defaultGameSettings();
		printf("Settings file not found, using defaults.\n");
	}

	initGame(&g, settings);
	initHandCards(&g, settings);
	sortHands(&g, settings, 1);
	compactDeck(g.deck);
	if (settings.splitDeck) {
		compactDeck(g.splitDeck);
	}
	determineWhoStarts(&g);
	runPreGameSwap(&g, settings);
	printf("\n%s starts!\n", g.stats.whoseTurn == 0 ? "You" : "AI");
	runGameLoop(&g, settings, slot);
}

static void loadAndResumeGame(void) {
	while (1) {
		printf("\nLoad Game:\n");
		for (uint8_t i = 1; i <= saveSlotCount; i++) {
			if (!slotExists(i)) {
				printf("  %u. Slot %u: empty\n", (unsigned)i, (unsigned)i);
			} else if (slotIsFinished(i)) {
				printf("  %u. Slot %u: finished\n", (unsigned)i, (unsigned)i);
			} else {
				printf("  %u. Slot %u: in progress\n", (unsigned)i, (unsigned)i);
			}
		}
		printf("  4. Back\n");
		printf("Choice: ");
		fflush(stdout);

		char buf[8];
		if (!fgets(buf, sizeof(buf), stdin)) return;
		int choice = atoi(buf);

		if (choice == 4) return;
		if (choice < 1 || choice > saveSlotCount) {
			printf("Invalid choice.\n");
			continue;
		}

		uint8_t slot = (uint8_t)choice;
		if (!slotExists(slot)) {
			printf("No save in slot %u.\n", (unsigned)slot);
			continue;
		}
		if (slotIsFinished(slot)) {
			printf("Slot %u is a finished game.\n", (unsigned)slot);
			continue;
		}

		game g;
		gameSettings settings = {0};
		if (loadSlot(slot, &g, &settings) != 0) {
			printf("Failed to load slot %u.\n", (unsigned)slot);
			continue;
		}

		printf("Loaded slot %u. Resuming...\n", (unsigned)slot);
		runGameLoop(&g, settings, slot);
		return;
	}
}

void runMainMenu(void) {
	while (1) {
		printf("\n=== ShitHead ===\n");
		printf("1. New Game\n");
		printf("2. Load Game\n");
		printf("3. Settings\n");
		printf("4. Rules\n");
		printf("5. Quit\n");
		printf("Choice: ");
		fflush(stdout);

		char buf[8];
		if (!fgets(buf, sizeof(buf), stdin)) break;

		switch (atoi(buf)) {
		case 1: startNewGame(); break;
		case 2: loadAndResumeGame(); break;
		case 3: runSettingsMenu(); break;
		case 4: printRules(); break;
		case 5: return;
		default: printf("Invalid choice.\n");
		}
	}
}
