# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Sudoku solver written in C. Single-file project (`sdk.c`, ~560 lines) that uses constraint propagation with hidden singles detection to solve 9x9 puzzles. Comments are in Portuguese.

## Build

```bash
gcc sdk.c -o sdk
```

No Makefile or build system exists. No external dependencies beyond the C standard library (`stdio.h`, `stdlib.h`).

## Architecture

The solver uses a 9x9 global matrix `S[T][T]` where each cell is a `data` struct containing:
- `int n` — the placed value (0 if unsolved)
- `struct dados *NoS` — linked list of candidate values (1–9)

**Solving algorithm:** When a value is placed, `check()` removes it from all peers (same row, column, 3x3 box) via `rem()`. If removal leaves a single candidate, that value is recursively placed. `perc_subG()` detects hidden singles within 3x3 boxes. `solve()` iterates through all boxes and `start()` runs up to 20 iterations until all 81 cells are filled.

**Limitation:** No backtracking — cannot solve puzzles that require guessing/search.

**Execution:** Interactive console menu — option 1 inserts puzzle clues, option 2 solves, option 0 exits.

## Code Notes

- Pre-C99 style: missing function prototypes, implicit return types
- Uses `fflush(stdin)` (non-standard)
- Global state (`S`, `count`) — no parameterized interfaces
- Manual memory management with malloc/free for candidate linked lists

## sudoku solver algorithms
- sdk.c - original code with your implementations
- sdk_bck.c - Uses simple backtracking
- sdk_mrv - Backtracking optimized with MRV
- sdk_dlx - Implementation using Dancing Links (DLX)

