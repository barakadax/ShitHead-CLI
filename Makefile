CC = gcc
CFLAGS = -static
LIBS = -lsodium -lm -lpthread
SRC = src/main.c src/debug.c
TARGET = a

.PHONY: all clean check_deps force

all: check_deps $(TARGET)

force:

$(TARGET): force
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LIBS)

check_deps:
	@echo "Checking for external dependencies..."
	@echo "int main() { return 0; }" | $(CC) -x c - -static -lsodium -o /dev/null >/dev/null 2>&1 || (echo "Error: libsodium not found or static linking failed."; exit 1)
	@echo "Dependencies valid."

clean:
	rm -f $(TARGET)
