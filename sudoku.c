// sudoku_solver.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h> // Added for time measurement

#define N 9
#define ALL_DIGITS_MASK 0x1FF  // 9 bits set: digits 1..9

static int grid[N][N];
static int rowMask[N], colMask[N], boxMask[N];

static int box_index(int r, int c) {
    return (r / 3) * 3 + (c / 3);
}

static int popcount9(int x) {
#if defined(__GNUC__) || defined(__clang__)
    return __builtin_popcount((unsigned)x);
#else
    int c = 0;
    while (x) { x &= (x - 1); c++; }
    return c;
#endif
}

static int digit_from_bit(int bit) {
#if defined(__GNUC__) || defined(__clang__)
    return __builtin_ctz((unsigned)bit) + 1; // bit 0 => digit 1
#else
    int d = 1;
    while ((bit & 1) == 0) { bit >>= 1; d++; }
    return d;
#endif
}

static void print_grid(void) {
    for (int r = 0; r < N; r++) {
        for (int c = 0; c < N; c++) {
            putchar('0' + grid[r][c]);
        }
        putchar('\n');
    }
}

static int init_masks_and_validate(void) {
    memset(rowMask, 0, sizeof(rowMask));
    memset(colMask, 0, sizeof(colMask));
    memset(boxMask, 0, sizeof(boxMask));

    for (int r = 0; r < N; r++) {
        for (int c = 0; c < N; c++) {
            int v = grid[r][c];
            if (v == 0) continue;
            if (v < 1 || v > 9) return 0;

            int bit = 1 << (v - 1);
            int b = box_index(r, c);

            if ((rowMask[r] & bit) || (colMask[c] & bit) || (boxMask[b] & bit)) {
                return 0; // conflict in given puzzle
            }

            rowMask[r] |= bit;
            colMask[c] |= bit;
            boxMask[b] |= bit;
        }
    }
    return 1;
}

// Find the empty cell with the fewest candidates (MRV heuristic).
// Returns:
//  1 if found an empty cell (and sets outR/outC/outCandBits),
//  0 if a dead-end was found (some empty cell has 0 candidates),
// -1 if puzzle is already complete (no empty cells).
static int choose_cell(int *outR, int *outC, int *outCandBits) {
    int bestCount = 10;
    int found = 0;

    for (int r = 0; r < N; r++) {
        for (int c = 0; c < N; c++) {
            if (grid[r][c] != 0) continue;

            int b = box_index(r, c);
            int used = rowMask[r] | colMask[c] | boxMask[b];
            int cand = (~used) & ALL_DIGITS_MASK;
            int cnt = popcount9(cand);

            if (cnt == 0) return 0; // dead end

            if (cnt < bestCount) {
                bestCount = cnt;
                *outR = r;
                *outC = c;
                *outCandBits = cand;
                found = 1;
                if (cnt == 1) return 1; // can't do better than this
            }
        }
    }

    return found ? 1 : -1;
}

static int solve(void) {
    int r, c, cand;
    int st = choose_cell(&r, &c, &cand);
    if (st == -1) return 1; // solved
    if (st == 0)  return 0; // dead end

    int b = box_index(r, c);

    while (cand) {
        int bit = cand & -cand;      // lowest set bit
        int d = digit_from_bit(bit); // digit 1..9

        // place
        grid[r][c] = d;
        rowMask[r] |= bit;
        colMask[c] |= bit;
        boxMask[b] |= bit;

        if (solve()) return 1;

        // undo
        grid[r][c] = 0;
        rowMask[r] &= ~bit;
        colMask[c] &= ~bit;
        boxMask[b] &= ~bit;

        cand &= (cand - 1); // clear lowest set bit
    }

    return 0;
}

static int read_puzzle_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return 0;

    char line[256];
    int r = 0;

    while (r < N && fgets(line, sizeof(line), f)) {
        int c = 0;
        for (size_t i = 0; line[i] != '\0' && c < N; i++) {
            unsigned char ch = (unsigned char)line[i];
            if (isdigit(ch)) {
                grid[r][c++] = (int)(ch - '0');
            } else if (ch == '.') {
                grid[r][c++] = 0;
            } else {
                // ignore other chars (spaces, tabs, etc.)
            }
        }

        if (c == 0) continue;      // skip empty lines
        if (c != N) { fclose(f); return 0; } // malformed row
        r++;
    }

    fclose(f);
    return (r == N);
}

int main(int argc, char **argv) {

    if (argc != 2) {
        fprintf(stderr, "Usage: %s puzzle.txt\n", argv[0]);
        return 2;
    }

    if (!read_puzzle_file(argv[1])) {
        fprintf(stderr, "Error: could not read a 9x9 puzzle from '%s'\n", argv[1]);
        return 2;
    }

    if (!init_masks_and_validate()) {
        fprintf(stderr, "Error: puzzle has conflicts (invalid Sudoku)\n");
        return 2;
    }

      // --- Time Measurement Start ---
    clock_t start_time = clock();
    
    int solved = solve();
    
    clock_t end_time = clock();
    double time_taken = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    if (!solved) {
        fprintf(stderr, "No solution found.\n");
        return 1;
    }

    print_grid();
    printf("Time taken to solve: %f seconds\n", time_taken);
    return 0;
}
