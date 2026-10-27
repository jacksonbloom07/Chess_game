# Terminal Chess

A command-line chess game written in C++17. The project models each chess piece
with inheritance and polymorphism, validates moves against the board state, and
accepts a useful subset of standard algebraic chess notation.

## Features

- Object-oriented design with a `Piece` base class and concrete `Rook`, `Knight`,
  `Bishop`, `Queen`, `King`, and `Pawn` classes
- Move generation for each piece type
- Turn-based two-player terminal interface
- Board rendering with Unicode chess pieces
- Captures and material tracking
- Check detection and basic king-safety validation
- Checkmate and Stalemate validation
- Parsing for common algebraic move notation, including captures and basic
  disambiguation (for example, `e4`, `Nf3`, `Bxc7`, and `Rae1`)
- En passant support
- Support for pawn promotion, castling, and long castling

## Build and run

Requirements: a C++17-compatible compiler and `make`.

```bash
make
./chess
```

Enter moves one at a time. Type `print` to display the board again, or `end` to
exit.

Example opening:

```text
e4
e5
Nf3
Nc6
```

## Project structure

| File | Purpose |
| --- | --- |
| `chess.cpp` | Command-line game loop |
| `board.h/.cpp` | Board state, move validation, captures, and notation parsing |
| `pieces.h/.cpp` | Piece hierarchy and movement logic |
| `player.h/.cpp` | Player state and captured material |
| `square.h` | Board-coordinate representation |
| `Makefile` | C++17 build instructions |

## Planned improvements

- Add automated unit tests for move legality and special moves
- Replace manual ownership with modern C++ smart pointers
- Add a graphical or web-based interface

## Notes

This is an educational project built to practice C++ object-oriented design,
dynamic polymorphism, parsing, and stateful rule validation.
