CC = gcc
CFLAGS_BASE = -std=gnu23 -static
CFLAGS_RELEASE = $(CFLAGS_BASE) -O3 -s -DNDEBUG
CFLAGS_DEBUG = $(CFLAGS_BASE) -g -O0 -DDEBUG
LIBS = -lsodium -lm -lpthread
SRC = src/main.c src/settings_loader.c
DBG_SRC =
TARGET = shithead

BUILD ?= debug

ifeq ($(BUILD),release)
BUILD_FLAGS = $(CFLAGS_RELEASE)
DBG_SRC =
else ifeq ($(BUILD),debug)
BUILD_FLAGS = $(CFLAGS_DEBUG)
DBG_SRC = src/debug.c
else
$(error Unknown BUILD '$(BUILD)'; valid values: release or debug)
endif

.PHONY: all clean check_deps force

all: check_deps build

force:

build: force
	$(CC) $(BUILD_FLAGS) $(SRC) $(DBG_SRC) -o $(TARGET) $(LIBS)

release: BUILD=release
release: build

debug: BUILD=debug
debug: build

check_deps:
	@echo "Checking for external dependencies..."
	@echo "int main() { return 0; }" | $(CC) -x c - -static -lsodium -o /dev/null >/dev/null 2>&1 || (echo "Error: libsodium not found or static linking failed."; exit 1)
	@echo "Dependencies valid."

clean:
	rm -f $(TARGET)
