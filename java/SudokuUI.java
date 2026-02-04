import javax.swing.*;
import javax.swing.border.Border;
import javax.swing.border.MatteBorder;
import java.awt.*;
import java.io.File;

public class SudokuUI extends JFrame {

    private static final int N = 9;
    private JTextField[][] cells = new JTextField[N][N];
    private boolean[][] given = new boolean[N][N];
    private SudokuSolver solver = new SudokuSolver();
    private JLabel statusLabel;
    private JButton solveBtn, loadBtn, clearBtn;

    public SudokuUI() {
        setTitle("Sudoku Solver - MRV Backtracking");
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setResizable(false);
        setLayout(new BorderLayout(0, 10));

        add(buildGridPanel(), BorderLayout.CENTER);
        add(buildControlPanel(), BorderLayout.SOUTH);

        pack();
        setLocationRelativeTo(null);
    }

    private JPanel buildGridPanel() {
        JPanel gridPanel = new JPanel(new GridLayout(N, N, 0, 0));
        gridPanel.setBorder(BorderFactory.createLineBorder(Color.BLACK, 2));

        Font cellFont = new Font("SansSerif", Font.BOLD, 22);

        for (int r = 0; r < N; r++) {
            for (int c = 0; c < N; c++) {
                JTextField tf = new JTextField(1);
                tf.setFont(cellFont);
                tf.setHorizontalAlignment(JTextField.CENTER);
                tf.setPreferredSize(new Dimension(50, 50));
                tf.setBorder(buildCellBorder(r, c));
                tf.setBackground(cellBackground(r, c));
                cells[r][c] = tf;
                gridPanel.add(tf);
            }
        }
        return gridPanel;
    }

    private Border buildCellBorder(int r, int c) {
        int top = (r % 3 == 0) ? 2 : 1;
        int left = (c % 3 == 0) ? 2 : 1;
        int bottom = (r == N - 1) ? 2 : 0;
        int right = (c == N - 1) ? 2 : 0;
        return new MatteBorder(top, left, bottom, right, Color.BLACK);
    }

    private Color cellBackground(int r, int c) {
        int boxRow = r / 3;
        int boxCol = c / 3;
        return (boxRow + boxCol) % 2 == 0
            ? new Color(240, 240, 255)
            : Color.WHITE;
    }

    private JPanel buildControlPanel() {
        JPanel panel = new JPanel(new BorderLayout(0, 5));
        panel.setBorder(BorderFactory.createEmptyBorder(0, 10, 10, 10));

        JPanel buttons = new JPanel(new FlowLayout(FlowLayout.CENTER, 15, 0));
        loadBtn = new JButton("Load");
        solveBtn = new JButton("Solve");
        clearBtn = new JButton("Clear");

        loadBtn.addActionListener(e -> loadPuzzle());
        solveBtn.addActionListener(e -> solvePuzzle());
        clearBtn.addActionListener(e -> clearPuzzle());

        buttons.add(loadBtn);
        buttons.add(solveBtn);
        buttons.add(clearBtn);

        statusLabel = new JLabel("Ready", SwingConstants.CENTER);
        statusLabel.setFont(new Font("SansSerif", Font.PLAIN, 13));

        panel.add(buttons, BorderLayout.NORTH);
        panel.add(statusLabel, BorderLayout.SOUTH);
        return panel;
    }

    private void loadPuzzle() {
        JFileChooser fc = new JFileChooser(System.getProperty("user.dir"));
        fc.setCurrentDirectory(new File(System.getProperty("user.dir")).getParentFile());
        if (fc.showOpenDialog(this) == JFileChooser.APPROVE_OPTION) {
            try {
                solver.loadFromFile(fc.getSelectedFile().getAbsolutePath());
                syncGridToUI();
                markGivens();
                statusLabel.setText("Puzzle loaded");
            } catch (Exception ex) {
                statusLabel.setText("Error: " + ex.getMessage());
            }
        }
    }

    private void solvePuzzle() {
        readUIToGrid();
        markGivens();
        setButtonsEnabled(false);
        statusLabel.setText("Solving...");

        SwingWorker<Boolean, Void> worker = new SwingWorker<>() {
            long elapsed;

            @Override
            protected Boolean doInBackground() {
                long t0 = System.nanoTime();
                boolean result = solver.solve();
                elapsed = System.nanoTime() - t0;
                return result;
            }

            @Override
            protected void done() {
                try {
                    boolean solved = get();
                    if (solved) {
                        syncGridToUI();
                        double ms = elapsed / 1_000_000.0;
                        statusLabel.setText(String.format("Solved in %.2f ms", ms));
                    } else {
                        statusLabel.setText("No solution exists");
                    }
                } catch (Exception ex) {
                    statusLabel.setText("Error: " + ex.getMessage());
                }
                setButtonsEnabled(true);
            }
        };
        worker.execute();
    }

    private void clearPuzzle() {
        solver.clear();
        for (int r = 0; r < N; r++) {
            for (int c = 0; c < N; c++) {
                cells[r][c].setText("");
                cells[r][c].setForeground(Color.BLACK);
                cells[r][c].setEditable(true);
                given[r][c] = false;
            }
        }
        statusLabel.setText("Ready");
    }

    private void readUIToGrid() {
        int[][] grid = solver.getGrid();
        for (int r = 0; r < N; r++) {
            for (int c = 0; c < N; c++) {
                String text = cells[r][c].getText().trim();
                if (!text.isEmpty()) {
                    try {
                        int val = Integer.parseInt(text);
                        grid[r][c] = (val >= 1 && val <= 9) ? val : 0;
                    } catch (NumberFormatException e) {
                        grid[r][c] = 0;
                    }
                } else {
                    grid[r][c] = 0;
                }
            }
        }
    }

    private void syncGridToUI() {
        int[][] grid = solver.getGrid();
        for (int r = 0; r < N; r++) {
            for (int c = 0; c < N; c++) {
                if (grid[r][c] != 0) {
                    cells[r][c].setText(String.valueOf(grid[r][c]));
                    if (!given[r][c]) {
                        cells[r][c].setForeground(new Color(0, 80, 180));
                    }
                } else {
                    cells[r][c].setText("");
                }
            }
        }
    }

    private void markGivens() {
        int[][] grid = solver.getGrid();
        for (int r = 0; r < N; r++) {
            for (int c = 0; c < N; c++) {
                if (grid[r][c] != 0) {
                    given[r][c] = true;
                    cells[r][c].setForeground(Color.BLACK);
                    cells[r][c].setEditable(false);
                } else {
                    given[r][c] = false;
                    cells[r][c].setForeground(Color.BLACK);
                    cells[r][c].setEditable(true);
                }
            }
        }
    }

    private void setButtonsEnabled(boolean enabled) {
        loadBtn.setEnabled(enabled);
        solveBtn.setEnabled(enabled);
        clearBtn.setEnabled(enabled);
    }
}
