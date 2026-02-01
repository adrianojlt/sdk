// Uses simple Backtracking 
// Fills cells in a fixed order (top-left to bottom-right)
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h> // Added for time measurement

#define N 9

// Function to print the Sudoku grid
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

// Check if it is safe to assign a number to grid[row][col]
bool isSafe(int grid[N][N], int row, int col, int num) {

    // 1. Check if 'num' is already in the current row
    for (int x = 0; x < N; x++) {
        if (grid[row][x] == num) return false;
    }

    // 2. Check if 'num' is already in the current column
    for (int x = 0; x < N; x++) {
        if (grid[x][col] == num) return false;
    }

    // 3. Check if 'num' is in the 3x3 subgrid
    int startRow = row - row % 3;
    int startCol = col - col % 3;
    
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (grid[i + startRow][j + startCol] == num) return false;
        }
    }

    return true;
}

// The Backtracking Solver function
bool solveSudoku(int grid[N][N]) {

    int row, col;
    bool isEmpty = false;

    // Find the first empty location (represented by 0)
    for (row = 0; row < N; row++) {
        for (col = 0; col < N; col++) {
            if (grid[row][col] == 0) {
                isEmpty = true;
                break;
            }
        }
        if (isEmpty) break;
    }

    // If no empty location is found, the puzzle is solved
    if (!isEmpty) return true;

    // Try numbers 1 through 9
    for (int num = 1; num <= 9; num++) {
        if (isSafe(grid, row, col, num)) {

            // Assign the number tentatively
            grid[row][col] = num;

            // Recurse to see if this assignment leads to a solution
            if (solveSudoku(grid)) return true;

            // FAILURE: Unassign (Backtrack) and try the next number
            grid[row][col] = 0;
        }
    }

    // Triggers backtracking
    return false;
}

// Function to read the file
void readFromFile(char *filename, int grid[N][N]) {

    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    // Logic to read dense digits (e.g., "003...")
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {

            // %1d reads exactly one digit, skipping whitespace/newlines
            if (fscanf(file, "%1d", &grid[i][j]) != 1) {
                fprintf(stderr, "Error: Invalid file format or not enough numbers.\n");
                fclose(file);
                exit(EXIT_FAILURE);
            }
        }
    }

    fclose(file);
}

int main(int argc, char *argv[]) {

    // Check if filename argument is provided
    if (argc != 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    int grid[N][N];

    // Read the puzzle
    readFromFile(argv[1], grid);

    printf("Input Puzzle:\n");
    printGrid(grid);

     // --- Time Measurement Start ---
    clock_t start_time = clock();
    
    bool solved = solveSudoku(grid);
    
    clock_t end_time = clock();
    double time_taken = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    // --- Time Measurement End ---

    if (solved) {
        printf("\nSolved Puzzle:\n");
        printGrid(grid);
    } else {
        printf("\nNo solution exists for this puzzle.\n");
    }

    printf("Time taken to solve: %f seconds\n", time_taken);

    return 0;
}