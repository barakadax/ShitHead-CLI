#ifndef SETTINGS_LOADER_H
#define SETTINGS_LOADER_H

#include "game.h"

int loadSettings(gameSettings* settings, const char* filename);
int saveSettings(const gameSettings* settings, const char* filename);

#endif
