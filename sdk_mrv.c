// Minimum Remaining Values (MRV), also known as the "Most Constrained Variable" heuristic, 
// improves backtracking by dynamically choosing which cell to fill next.
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define N 9

// Struct to return multiple values from the MRV search
typedef struct {
    int row;
    int col;
    bool found;
} Position;

void printGrid(int grid[N][N]) {
    printf("-------------------------\n");
    for (int row = 0; row < N; row++) {
        if (row % 3 == 0 && row != 0)
            printf("-------------------------\n");
        for (int col = 0; col < N; col++) {
            if (col % 3 == 0 && col != 0)
                printf("| ");
            printf("%d ", grid[row][col]);
        }
        printf("\n");
    }
    printf("-------------------------\n");
}

// Function to check valid moves
bool isSafe(int grid[N][N], int row, int col, int num) {
    // Check row
    for (int x = 0; x < N; x++)
        if (grid[row][x] == num) return false;

    // Check column
    for (int x = 0; x < N; x++)
        if (grid[x][col] == num) return false;

    // Check 3x3 box
    int startRow = row - row % 3;
    int startCol = col - col % 3;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (grid[i + startRow][j + startCol] == num) return false;

    return true;
}

// --- MRV Heuristic ---
// Scans the whole grid to find the empty cell with the FEWEST valid options.
Position findBestCell(int grid[N][N]) {
    Position bestPos = {-1, -1, false};
    int minOptions = N + 1; // Start higher than max possible (9)

    for (int r = 0; r < N; r++) {
        for (int c = 0; c < N; c++) {
            if (grid[r][c] == 0) {
                // Count valid options for this specific empty cell
                int optionsCount = 0;
                for (int num = 1; num <= 9; num++) {
                    if (isSafe(grid, r, c, num)) {
                        optionsCount++;
                    }
                }

                // If this cell has fewer options than our current best, pick it
                if (optionsCount < minOptions) {
                    minOptions = optionsCount;
                    bestPos.row = r;
                    bestPos.col = c;
                    bestPos.found = true;
                    
                    // Optimization: If a cell has 0 or 1 options, we can't beat that priority.
                    // (0 means dead end, 1 means forced move). Stop searching.
                    if (minOptions <= 1) return bestPos;
                }
            }
        }
    }
    return bestPos;
}

bool solveSudoku(int grid[N][N]) {
    // Step 1: Use MRV to find the best cell to fill next
    Position pos = findBestCell(grid);

    // If no empty cell was found, the puzzle is solved!
    if (!pos.found) return true;

    int row = pos.row;
    int col = pos.col;

    // Step 2: Try numbers 1-9 for this best cell
    for (int num = 1; num <= 9; num++) {
        if (isSafe(grid, row, col, num)) {
            grid[row][col] = num;

            if (solveSudoku(grid)) return true;

            grid[row][col] = 0; // Backtrack
        }
    }

    return false;
}

void readFromFile(char *filename, int grid[N][N]) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (fscanf(file, "%1d", &grid[i][j]) != 1) {
                fprintf(stderr, "Error reading file.\n");
                exit(EXIT_FAILURE);
            }
        }
    }
    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    int grid[N][N];
    readFromFile(argv[1], grid);

    printf("Input Puzzle:\n");
    printGrid(grid);

    clock_t start = clock();
    bool solved = solveSudoku(grid);
    clock_t end = clock();

    if (solved) {
        printf("\nSolved Puzzle (with MRV):\n");
        printGrid(grid);
    } else {
        printf("\nNo solution exists.\n");
    }

    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Time taken: %f seconds\n", time_taken);

    return 0;
}
