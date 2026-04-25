#include "ui/menu.h"
#include <sodium.h>
#include <stdio.h>
#ifdef _WIN32
#  include <windows.h>
#endif

int main(void) {
#ifdef _WIN32
	SetConsoleOutputCP(CP_UTF8);
#endif
	if (sodium_init() < 0) {
		printf("Sodium initialization failed\n");
		return 1;
	}
	runMainMenu();
	return 0;
}
