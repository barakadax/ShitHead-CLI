# ShitHead
Implementation of the card game called "ShitHead" in C.
The game is also called: Karma, Palace, Shed.

The game is played with a standard 52-card deck 1 on 1 against the computer.

## Local dependencies
```shell
# Linux
sudo apt-get install libsodium-dev

# MacOS
brew install libsodium
```

## Building
```shell
# Build (default: debug)
make

# Build explicitly in release mode (same as default)
make BUILD=release

# Build debug (no optimizations, includes debug symbols)
make BUILD=debug

# Or use convenience targets
make release
make debug

# Clean
make clean
```

## TODO
- format settings of my liking
- save settings as JSON
- start with easy logic
