import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;

public class SudokuSolver {

    private static final int N = 9;
    private int[][] grid;

    public SudokuSolver() {
        grid = new int[N][N];
    }

    public int[][] getGrid() {
        return grid;
    }

    public void setGrid(int[][] source) {
        for (int i = 0; i < N; i++) {
            System.arraycopy(source[i], 0, grid[i], 0, N);
        }
    }

    public void clear() {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                grid[i][j] = 0;
            }
        }
    }

    public void loadFromFile(String path) throws IOException {
        try (BufferedReader br = new BufferedReader(new FileReader(path))) {
            int row = 0;
            String line;
            while ((line = br.readLine()) != null && row < N) {
                line = line.trim();
                if (line.isEmpty()) continue;
                int col = 0;
                for (int i = 0; i < line.length() && col < N; i++) {
                    char c = line.charAt(i);
                    if (c >= '0' && c <= '9') {
                        grid[row][col] = c - '0';
                        col++;
                    } else if (c == '.') {
                        grid[row][col] = 0;
                        col++;
                    }
                }
                row++;
            }
        }
    }

    private boolean isSafe(int row, int col, int num) {
        for (int i = 0; i < N; i++) {
            if (grid[row][i] == num) return false;
            if (grid[i][col] == num) return false;
        }
        int boxRow = (row / 3) * 3;
        int boxCol = (col / 3) * 3;
        for (int r = boxRow; r < boxRow + 3; r++) {
            for (int c = boxCol; c < boxCol + 3; c++) {
                if (grid[r][c] == num) return false;
            }
        }
        return true;
    }

    private int countCandidates(int row, int col) {
        int count = 0;
        for (int num = 1; num <= N; num++) {
            if (isSafe(row, col, num)) count++;
        }
        return count;
    }

    /**
     * MRV heuristic: find empty cell with fewest valid candidates.
     * Returns {row, col} or null if no empty cell (puzzle solved).
     */
    private int[] findBestCell() {
        int bestRow = -1, bestCol = -1;
        int minCandidates = N + 1;

        for (int r = 0; r < N; r++) {
            for (int c = 0; c < N; c++) {
                if (grid[r][c] == 0) {
                    int count = countCandidates(r, c);
                    if (count == 0) return new int[]{r, c, 0}; // dead end
                    if (count < minCandidates) {
                        minCandidates = count;
                        bestRow = r;
                        bestCol = c;
                        if (count == 1) return new int[]{bestRow, bestCol, 1};
                    }
                }
            }
        }
        if (bestRow == -1) return null; // solved
        return new int[]{bestRow, bestCol, minCandidates};
    }

    public boolean solve() {
        int[] best = findBestCell();
        if (best == null) return true; // all cells filled
        int row = best[0];
        int col = best[1];
        int candidates = best[2];
        if (candidates == 0) return false; // dead end

        for (int num = 1; num <= N; num++) {
            if (isSafe(row, col, num)) {
                grid[row][col] = num;
                if (solve()) return true;
                grid[row][col] = 0;
            }
        }
        return false;
    }
}
