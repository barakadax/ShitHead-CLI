# ShitHead

CLI implementation of the card game ShitHead (also known as Karma, Palace, or Shed) written in C (gnu23).

1-vs-1 against the computer. The loser is the ShitHead.

---

## How to Play

Each player starts with 3 face-down cards, 3 face-up cards on top of them, and 3 cards in hand.

**Turn order:** play from hand first. Once your hand is empty and the deck is exhausted, play your face-up cards. Once those are gone, play your face-down cards blind — if the revealed card is illegal you must pick up the whole pile.

After each hand-phase turn, draw from the deck until you hold 3 cards again.

**Playing a card:** play a card equal to or higher than the top of the pile. Multiple cards of the same value can be played in one move.

**Picking up:** if you cannot play (or choose to), take the entire pile into your hand.

**Win condition:** first player to empty their hand, face-up cards, and face-down cards wins.

### Magic Cards

| Card | Effect |
|------|--------|
| 2 | Resets the pile — any card can follow |
| 3 | Transparent — the next card plays against the card beneath the 3 |
| 7 | *(if enabled)* The next card played must be ≤ 7 |
| 8 | *(if enabled)* Play again |
| 10 | Burns the pile; play again on an empty pile |
| A | Only A, 2, 3, or 10 can follow |
| 4-of-a-kind on top | Burns the pile; play again |

3-on-8 rule *(if enabled)*: playing a 3 on an 8 keeps the 3 transparent **and** grants another turn.

---

## Settings

Edit `settings.json` in the project root before starting a new game. Each save slot gets its own copy at creation time.

| Key | Values | Description |
|-----|--------|-------------|
| `splitDeck` | 0/1 | Player draws from one half of the deck, AI from the other |
| `magicNumberSeven` | 0/1 | Enable the 7 ≤ 7 rule |
| `tenOnSeven` | 0/1 | Allow 10 to be played on 7 (overrides 7-rule block) |
| `magicNumberEight` | 0/1 | Enable 8 = play again |
| `threeOnEight` | 0/1 | Enable 3-on-8 grants another turn |
| `playerAutoOrder` | 0/1 | Keep the player's hand sorted automatically |
| `hinting` | 0/1 | Highlight legal plays (1) or show all cards and reject illegal picks (0) |
| `allowVoluntaryPickup` | 0/1 | Let the player voluntarily pick up the pile on their turn |
| `aiDifficulty` | 0–3 | 0 = Easy, 1 = Medium, 2 = Hard, 3 = Cheater |
| `autoSave` | 0/1 | Auto-save to the slot before each player turn |

---

## Save Slots

Three save slots are available (`saves/slot1/`, `saves/slot2/`, `saves/slot3/`). Each slot stores a `state.json` (full game state) and a `settings.json` (settings at the time the slot was created). Load a slot from the main menu to resume exactly where you left off.

---

## AI Difficulties

| Difficulty | Strategy |
|------------|----------|
| Easy | Plays the highest valid standard card; falls back to magic cards (10 > 2 > 3) |
| Medium | Plays the largest set of the lowest valid value; falls back to magic cards (3 > 10 > 2) |
| Hard | Completes 4-of-a-kind burns first; uses known player cards to force pickups; plays lowest valid; falls back to magic (2 > 3 > 10) |
| Cheater | Same as Hard but reads the player's face-down cards directly |

---

## Building

### Linux (native)

**Install dependency:**
```shell
sudo apt-get install libsodium-dev
```

**Build:**
```shell
make              # debug build (default)
make release      # optimised, stripped
make debug        # explicit debug
make clean
```

---

### macOS (native)

**Install dependency:**
```shell
brew install libsodium
```

**Build:**
```shell
make macos              # debug build
make macos BUILD=release
make clean
```

The Makefile detects the Homebrew prefix automatically. `-static` is omitted because Apple does not ship static system libraries.

---

### Windows — native build via MSYS2 (recommended)

1. Install [MSYS2](https://www.msys2.org/).
2. Open the **MinGW64** shell and run:
```shell
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-libsodium make
```
3. From the MinGW64 shell, in the project directory:
```shell
make              # auto-detects Windows, produces shithead.exe
make release
make clean
```

---

### Windows — cross-compile from Linux

**Install toolchain:**
```shell
sudo apt-get install mingw-w64
```

**Install MinGW libsodium** (one of):
- Download a prebuilt `libsodium-*-mingw.tar.gz` from the [libsodium releases page](https://github.com/jedisct1/libsodium/releases) and extract into `/usr/x86_64-w64-mingw32/`.
- Build from source: `./configure --host=x86_64-w64-mingw32 && make && sudo make install`.

**Build:**
```shell
make windows              # debug, produces shithead.exe
make windows BUILD=release
make clean
```

> **Linux → macOS cross-compile is not supported.** Apple's SDK cannot be freely redistributed. Build the macOS binary on a Mac.

---

### Explicit OS override

Any target can be forced regardless of host:

```shell
make OS_TARGET=linux BUILD=release
make OS_TARGET=macos BUILD=debug
make OS_TARGET=windows BUILD=release
```

`check_deps` runs automatically on `release` and named OS targets and will print a clear error if libsodium is not found.

---

