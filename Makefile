ifeq ($(OS),Windows_NT)
    DETECTED_OS := windows
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Darwin)
        DETECTED_OS := macos
    else
        DETECTED_OS := linux
    endif
endif

OS_TARGET ?= $(DETECTED_OS)

CFLAGS_COMMON = -std=gnu23 -I src/
LIBS_COMMON = -lsodium -lm

ifeq ($(OS_TARGET),linux)
    CC ?= gcc
    TARGET = shithead
    PLATFORM_CFLAGS = -static
    PLATFORM_LDFLAGS =
    PLATFORM_LIBS = -lpthread
else ifeq ($(OS_TARGET),macos)
    CC ?= clang
    TARGET = shithead
    BREW_PREFIX := $(shell brew --prefix 2>/dev/null)
    ifneq ($(BREW_PREFIX),)
        PLATFORM_CFLAGS = -I$(BREW_PREFIX)/include
        PLATFORM_LDFLAGS = -L$(BREW_PREFIX)/lib
    else
        PLATFORM_CFLAGS =
        PLATFORM_LDFLAGS =
    endif
    PLATFORM_LIBS = -lpthread
else ifeq ($(OS_TARGET),windows)
    CC ?= x86_64-w64-mingw32-gcc
    TARGET = shithead.exe
    PLATFORM_CFLAGS = -static -D_WIN32_WINNT=0x0601
    PLATFORM_LDFLAGS =
    PLATFORM_LIBS = -lpthread -lbcrypt -lws2_32
else
    $(error Unknown OS_TARGET '$(OS_TARGET)'; valid: linux, macos, windows)
endif

CFLAGS_RELEASE = $(CFLAGS_COMMON) $(PLATFORM_CFLAGS) -O3 -s -DNDEBUG
CFLAGS_DEBUG = $(CFLAGS_COMMON) $(PLATFORM_CFLAGS) -g -O0 -DDEBUG
LIBS = $(LIBS_COMMON) $(PLATFORM_LIBS)

SRC = src/main.c \
      src/io/settings_loader.c src/io/save.c \
      src/core/card_rank.c src/core/deck.c src/core/pile.c src/core/rules.c src/core/starter.c \
      src/game/turn.c src/game/swap.c \
      src/ui/ui.c src/ui/hint.c src/ui/menu.c \
      src/ai/ai.c src/ai/ai_common.c src/ai/easy.c src/ai/medium.c src/ai/hard.c src/ai/cheater.c

DBG_SRC =

BUILD ?= debug

ifeq ($(BUILD),release)
BUILD_FLAGS = $(CFLAGS_RELEASE)
DBG_SRC =
else ifeq ($(BUILD),debug)
BUILD_FLAGS = $(CFLAGS_DEBUG)
DBG_SRC = src/debug/debug.c
else
$(error Unknown BUILD '$(BUILD)'; valid values: release or debug)
endif

.PHONY: all clean check_deps force release debug linux macos windows

all: check_deps build

force:

build: force
	$(CC) $(BUILD_FLAGS) $(SRC) $(DBG_SRC) -o $(TARGET) $(PLATFORM_LDFLAGS) $(LIBS)

release:
	$(MAKE) BUILD=release OS_TARGET=$(OS_TARGET) check_deps build

debug:
	$(MAKE) BUILD=debug OS_TARGET=$(OS_TARGET) build

linux:
	$(MAKE) OS_TARGET=linux BUILD=$(BUILD) check_deps build

macos:
	$(MAKE) OS_TARGET=macos BUILD=$(BUILD) check_deps build

windows:
	$(MAKE) OS_TARGET=windows BUILD=$(BUILD) check_deps build

check_deps:
	@echo "Checking dependencies for target: $(OS_TARGET)"
	@echo "int main(void) { return 0; }" | $(CC) -x c - $(PLATFORM_CFLAGS) $(PLATFORM_LDFLAGS) -lsodium -o /dev/null >/dev/null 2>&1 || \
	    (echo "Error: libsodium not found or linking failed for $(OS_TARGET). See readme.md for install instructions."; exit 1)
	@echo "Dependencies valid."

clean:
	rm -f shithead shithead.exe
