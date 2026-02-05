# Sudoku Solver

A collection of Sudoku solvers in C, exploring different algorithmic approaches — from constraint propagation to Dancing Links.

This project started as a university assignment: a basic solver in C that used constraint propagation with hidden singles detection. It worked for easy and moderate puzzles, but couldn't handle the hard ones. The missing piece was **backtracking** — something I only properly identified years later when I revisited the code with [Claude Code](https://adrianojlt.github.io/posts/2026/claude_code/). That session became an excuse to explore multiple solving algorithms side by side.

## Algorithms

| File | Algorithm | Description |
|------|-----------|-------------|
| `sdk.c` | Constraint Propagation + Backtracking | Original solver. Eliminates candidates from peers and detects hidden singles in 3x3 boxes. Falls back to MRV backtracking for harder puzzles. Interactive console menu. |
| `sdk_bck.c` | Simple Backtracking | Brute-force depth-first search. Fills cells left-to-right, top-to-bottom, trying digits 1-9. Simple and correct, but slow on hard puzzles. |
| `sdk_mrv.c` | MRV Backtracking | Backtracking improved with the Minimum Remaining Values heuristic — always fills the most constrained cell next, reducing the search space significantly. |
| `sudoku.c` | Bitwise MRV | Same MRV strategy but tracks candidates using bitmasks instead of iteration. Uses `__builtin_popcount` and `__builtin_ctz` for fast candidate counting and extraction. |
| `sdk_dlx.c` | Dancing Links (DLX) | Transforms Sudoku into an exact cover problem and solves it with Knuth's Algorithm X using circular doubly-linked lists. The most efficient approach for hard puzzles. |

There is also a **Java GUI** implementation in the `java/` directory.

## Build

No external dependencies — only the C standard library. Compile each solver individually with `gcc`:

```bash
gcc sdk.c -o sdk
gcc sdk_bck.c -o sdk_bck
gcc sdk_mrv.c -o sdk_mrv
gcc sdk_dlx.c -o sdk_dlx
gcc sudoku.c -o sudoku
```

## Usage

The original solver (`sdk.c`) has an interactive menu where you can insert clues manually or import from a file:

```bash
./sdk
```

All other solvers read a puzzle from a file passed as an argument:

```bash
./sdk_bck sdk.txt
./sdk_mrv hard.txt
./sdk_dlx hard.txt
./sudoku hard.txt
```

### Puzzle format

A text file with 9 lines of 9 digits each. `0` represents an empty cell:

```
003020600
900305001
001806400
008102900
700000008
006708200
002609500
800203009
005010300
```

Sample puzzles are included in the repo: `sdk.txt`, `sdk01.txt`, and `hard.txt`.

## Background

The original `sdk.c` was written as a university project — pre-C99 style, Portuguese comments, manual linked-list management for candidates, and no backtracking. It could only solve puzzles where constraint propagation alone was sufficient.

Revisiting it with Claude Code led to adding backtracking support to the original code, and then implementing the same problem with different algorithms to compare approaches. The progression from simple backtracking to MRV to bitwise optimization to DLX shows how algorithm choice directly impacts performance, especially on hard puzzles.

More on that story: [My first experience with Claude Code](https://adrianojlt.github.io/posts/2026/claude_code/)
