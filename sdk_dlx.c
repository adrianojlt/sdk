// This is a C implementation of Dancing Links (DLX), 
// the most efficient algorithm known for solving exact cover problems like Sudoku. 
// It is significantly more complex than backtracking because it transforms the puzzle 
// into a matrix of 729 rows and 324 columns and uses circular doubly-linked lists to "dance" through valid options.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_ROWS 729    // 9*9*9
#define MAX_COLS 324    // 81*4 constraints

// --- DLX Node Structure ---
typedef struct Node {
    struct Node *left;
    struct Node *right;
    struct Node *up;
    struct Node *down;
    struct Node *column; // Pointer to the column header
    int rowID;           // To identify which move this node represents
    int colID;           // To identify the constraint
    int nodeCount;       // Only used in column headers
} Node;

// Global Root
Node *root;
Node *columnHeaders[MAX_COLS];
Node nodes[MAX_ROWS * 4]; // Pre-allocated pool for matrix nodes
int nodeIdx = 0;

// Result storage
int result[81]; 
int resultCount = 0;
int solvedGrid[9][9];

// --- Initialization ---

// Create a new column header
Node* createColumnHeader(int id) {
    Node *n = (Node*)malloc(sizeof(Node));
    n->left = n->right = n->up = n->down = n;
    n->column = n;
    n->rowID = -1;
    n->colID = id;
    n->nodeCount = 0;
    return n;
}

// Create the Toroidal Matrix Headers
void initMatrix() {
    root = createColumnHeader(-1); // Master root
    Node *prev = root;

    for (int i = 0; i < MAX_COLS; i++) {
        Node *n = createColumnHeader(i);
        columnHeaders[i] = n;
        
        // Link horizontally
        n->left = prev;
        n->right = root;
        prev->right = n;
        root->left = n;
        
        prev = n;
    }
}

// Add a node to the matrix at (r, c)
void addNode(int r, int c) {
    // Get the column header
    Node *col = columnHeaders[c];
    
    // Allocate new node from pool
    Node *n = &nodes[nodeIdx++];
    n->rowID = r;
    n->colID = c;
    n->column = col;
    n->nodeCount = 0; // Not used for inner nodes

    // Link vertically (append to bottom of column)
    Node *bottom = col->up;
    n->down = col;
    n->up = bottom;
    bottom->down = n;
    col->up = n;
    
    col->nodeCount++;

    // Link horizontally (if this is not the first node in this row)
    // To do this, we need to find if there's already a node for this 'r'
    // Since we build the matrix row by row, we can keep track of "prev" in the loop calling this.
    // BUT, for simplicity in this helper, we'll assume the caller handles horizontal linking
    // or we can handle it if we store a "lastNodeForRow" array. 
    // Simplified approach: The caller (buildMatrix) will link them horizontally.
}

// --- DLX Operations ---

void cover(Node *c) {
    // Remove column c from header list
    c->right->left = c->left;
    c->left->right = c->right;

    // Remove all rows in this column
    for (Node *i = c->down; i != c; i = i->down) {
        for (Node *j = i->right; j != i; j = j->right) {
            j->down->up = j->up;
            j->up->down = j->down;
            j->column->nodeCount--;
        }
    }
}

void uncover(Node *c) {
    // Restore all rows
    for (Node *i = c->up; i != c; i = i->up) {
        for (Node *j = i->left; j != i; j = j->left) {
            j->column->nodeCount++;
            j->down->up = j;
            j->up->down = j;
        }
    }
    // Restore column c
    c->right->left = c;
    c->left->right = c;
}

// --- Solver ---

// Helper to decode rowID (0..728) back to (r, c, num)
void decodeRow(int rowID, int *r, int *c, int *num) {
    *r = rowID / 81;
    *c = (rowID / 9) % 9;
    *num = (rowID % 9) + 1;
}

int solve() {
    if (root->right == root) {
        // Solution found!
        return 1;
    }

    // Heuristic: Choose column with smallest nodeCount
    Node *c = root->right;
    int minCount = 999999;
    
    for (Node *temp = root->right; temp != root; temp = temp->right) {
        if (temp->nodeCount < minCount) {
            minCount = temp->nodeCount;
            c = temp;
        }
    }

    cover(c);

    for (Node *r = c->down; r != c; r = r->down) {
        // Choose row r
        result[resultCount++] = r->rowID;

        for (Node *j = r->right; j != r; j = j->right) {
            cover(j->column);
        }

        if (solve()) return 1;

        // Backtrack
        resultCount--;
        for (Node *j = r->left; j != r; j = j->left) {
            uncover(j->column);
        }
    }

    uncover(c);
    return 0;
}

// --- Matrix Construction ---

void buildMatrix(int initialGrid[9][9]) {
    nodeIdx = 0; // Reset pool
    initMatrix();

    // Loop through all possible placements (rows of the matrix)
    // RowID = r * 81 + c * 9 + (num - 1)
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            for (int num = 1; num <= 9; num++) {
                
                // If the grid is pre-filled, only allow the matching number
                if (initialGrid[r][c] != 0 && initialGrid[r][c] != num) {
                    continue; 
                }

                int rowID = r * 81 + c * 9 + (num - 1);
                
                // Calculate the 4 constraints (columns indices 0..323)
                // 1. Cell constraint: Cell (r,c) must have a number
                int idx1 = 0 * 81 + (r * 9 + c);
                // 2. Row constraint: Row r must have number 'num'
                int idx2 = 1 * 81 + (r * 9 + (num - 1));
                // 3. Col constraint: Col c must have number 'num'
                int idx3 = 2 * 81 + (c * 9 + (num - 1));
                // 4. Box constraint: Box b must have number 'num'
                int b = (r / 3) * 3 + (c / 3);
                int idx4 = 3 * 81 + (b * 9 + (num - 1));

                // Add these 4 nodes to the matrix and link them horizontally
                // We manually link them here to keep addNode simple
                
                // We'll create 4 nodes in our pool
                Node* n1 = &nodes[nodeIdx++];
                Node* n2 = &nodes[nodeIdx++];
                Node* n3 = &nodes[nodeIdx++];
                Node* n4 = &nodes[nodeIdx++];

                // Configure Node 1
                n1->rowID = rowID; n1->colID = idx1; n1->column = columnHeaders[idx1];
                Node *bottom1 = columnHeaders[idx1]->up;
                n1->down = columnHeaders[idx1]; n1->up = bottom1; bottom1->down = n1; columnHeaders[idx1]->up = n1;
                columnHeaders[idx1]->nodeCount++;

                // Configure Node 2
                n2->rowID = rowID; n2->colID = idx2; n2->column = columnHeaders[idx2];
                Node *bottom2 = columnHeaders[idx2]->up;
                n2->down = columnHeaders[idx2]; n2->up = bottom2; bottom2->down = n2; columnHeaders[idx2]->up = n2;
                columnHeaders[idx2]->nodeCount++;

                // Configure Node 3
                n3->rowID = rowID; n3->colID = idx3; n3->column = columnHeaders[idx3];
                Node *bottom3 = columnHeaders[idx3]->up;
                n3->down = columnHeaders[idx3]; n3->up = bottom3; bottom3->down = n3; columnHeaders[idx3]->up = n3;
                columnHeaders[idx3]->nodeCount++;

                // Configure Node 4
                n4->rowID = rowID; n4->colID = idx4; n4->column = columnHeaders[idx4];
                Node *bottom4 = columnHeaders[idx4]->up;
                n4->down = columnHeaders[idx4]; n4->up = bottom4; bottom4->down = n4; columnHeaders[idx4]->up = n4;
                columnHeaders[idx4]->nodeCount++;

                // Link Horizontally (n1 -> n2 -> n3 -> n4 -> n1)
                n1->right = n2; n1->left = n4;
                n2->right = n3; n2->left = n1;
                n3->right = n4; n3->left = n2;
                n4->right = n1; n4->left = n3;
            }
        }
    }
}

// --- IO and Main ---

void printGridFinal() {
    printf("-------------------------\n");
    for (int r = 0; r < 9; r++) {
        if (r % 3 == 0 && r != 0) printf("-------------------------\n");
        for (int c = 0; c < 9; c++) {
            if (c % 3 == 0 && c != 0) printf("| ");
            printf("%d ", solvedGrid[r][c]);
        }
        printf("\n");
    }
    printf("-------------------------\n");
}

void readFromFile(char *filename, int grid[9][9]) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (fscanf(file, "%1d", &grid[i][j]) != 1) {
                fprintf(stderr, "Error reading file format.\n");
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

    int inputGrid[9][9];
    readFromFile(argv[1], inputGrid);

    // Build the Exact Cover Matrix
    buildMatrix(inputGrid);

    printf("Running DLX Solver...\n");
    clock_t start = clock();
    
    int success = solve();
    
    clock_t end = clock();
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;

    if (success) {
        // Convert rowIDs back to grid
        for (int i = 0; i < resultCount; i++) {
            int r, c, num;
            decodeRow(result[i], &r, &c, &num);
            solvedGrid[r][c] = num;
        }
        printf("\nSolved Puzzle:\n");
        printGridFinal();
    } else {
        printf("\nNo solution found.\n");
    }

    printf("Time taken: %f seconds\n", time_taken);
    
    // Cleanup (Optional since OS reclaims memory, but good practice)
    for(int i=0; i<MAX_COLS; i++) free(columnHeaders[i]);
    free(root);

    return 0;
}
