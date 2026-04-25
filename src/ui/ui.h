#ifndef UI_H
#define UI_H

#include "game.h"

void printCard(card c);
void printBoard(const game* g);
void printPlayerHand(const game* g);
void printPlayerFaceUp(const game* g);
void printRules(void);
void promptEnterToContinue(void);

#endif
