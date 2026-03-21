#include "SudokuBoard.h"
#include <ctime>
#include <ncurses.h>
#include <string>

int main() {
  srand(time(0));
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(0);
  start_color();

  init_pair(1, COLOR_WHITE, COLOR_BLACK);  // empty
  init_pair(2, COLOR_CYAN, COLOR_BLACK);   // fixed
  init_pair(3, COLOR_GREEN, COLOR_BLACK);  // user
  init_pair(4, COLOR_YELLOW, COLOR_BLACK); // cursor
  init_pair(5, COLOR_RED, COLOR_BLACK);    // errors

  SudokuBoard sudoku;
  sudoku.generate(35);
  auto &board = sudoku.getPlayerBoard();

  //  board = {{{5, 3, 0, 0, 7, 0, 0, 0, 0},
  //          {6, 0, 0, 1, 9, 5, 0, 0, 0},
  //          {0, 9, 8, 0, 0, 0, 0, 6, 0},
  //          {8, 0, 0, 0, 6, 0, 0, 0, 3},
  //          {4, 0, 0, 8, 0, 3, 0, 0, 1},
  //          {7, 0, 0, 0, 2, 0, 0, 0, 6},
  //          {0, 6, 0, 0, 0, 0, 2, 8, 0},
  //          {0, 0, 0, 4, 1, 9, 0, 0, 5},
  //          {0, 0, 0, 0, 8, 0, 0, 7, 9}}};

  int cy = 0, cx = 0;
  std::string msg = "";

  bool solved = false;
  bool showErrors = false;

  while (true) {
    clear();

    mvprintw(1, 21, "=== SUDOKU SOLVER ===");

    // Draw cells
    for (int i = 0; i < 9; ++i) {
      for (int j = 0; j < 9; ++j) {
        int y = 4 + i + (i / 3); // +1 extra line every 3 rows
        int x = 18 + j * 3;      // 3 chars per cell + extra every 3 cols

        int color = 1; // default: empty cell (white)

        // 1. Fixed / given cells (cyan)
        if (sudoku.isCellFixed(i, j)) {
          color = 2;
        } else if (showErrors && !sudoku.isCorrect(i, j)) {
          color = 5; // red on black (default) for errors
        }

        // 2. Player-filled cells (green)
        else if (board[i][j] != 0) {
          color = 3;
        }
        // 3. Cursor gets highest priority (yellow + reverse)
        if (i == cy && j == cx) {
          color = 4;
        }

        attron(COLOR_PAIR(color));
        if (i == cy && j == cx) {
          attron(A_REVERSE);
        }

        mvprintw(y, x, "%c", board[i][j] == 0 ? '.' : '0' + board[i][j]);

        attroff(A_REVERSE | COLOR_PAIR(color));
      }
    }

    // Draw ALL horizontal borders (4 lines)
    for (int b = 0; b <= 3; ++b) {
      int y = 3 + b * 4;
      mvprintw(y, 17, "|--------|--------|--------|");
    }

    // Draw vertical borders
    for (int b = 0; b <= 3; ++b) {
      int x = 17 + b * 9;
      for (int i = 0; i < 13; ++i) {
        mvprintw(3 + i, x, "|");
      }
    }

    // Status
    mvprintw(17, 8, "%s", msg.c_str());
    mvprintw(19, 8,
             "Arrows Move  |  1-9 Fill  |  0 delete  |  s Solve  |  g Generate "
             " |  q Quit");
    if (solved) {
      attron(A_BOLD | COLOR_PAIR(3));
      mvprintw(24, 23, "PUZZLE SOLVED!");
      attroff(A_BOLD | COLOR_PAIR(3));
    }

    refresh();

    int ch = getch();

    if (ch == 'q' || ch == 'Q')
      break;

    if (ch == KEY_UP && cy > 0)
      cy--;
    if (ch == KEY_DOWN && cy < 8)
      cy++;
    if (ch == KEY_LEFT && cx > 0)
      cx--;
    if (ch == KEY_RIGHT && cx < 8)
      cx++;

    if (ch >= '1' && ch <= '9') {
      int val = ch - '0';

      if (!sudoku.isCellFixed(cy, cx)) {
        sudoku.getPlayerBoard()[cy][cx] = val;
        msg = "Number placed.";
      }
    }
    // Check for win condition
    bool filled = true;
    bool correct = true;
    for (int i = 0; i < 9; ++i) {
      for (int j = 0; j < 9; ++j) {
        if (board[i][j] == 0)
          filled = false;
        if (!sudoku.isCorrect(i, j))
          correct = false;
      }
    }

    if (filled && correct)
      solved = true;

    if (ch == '0' || ch == KEY_DC || ch == KEY_BACKSPACE) {
      if (!sudoku.isCellFixed(cy, cx)) {
        sudoku.getPlayerBoard()[cy][cx] = 0;
        msg = "Cleared.";
      }
    }

    if (ch == 's' || ch == 'S') {
      msg = "Solving...";
      refresh();
      if (sudoku.solve()) {
        sudoku.revealSolution();
        solved = true;
        msg = "Solved successfully!";
      } else {
        msg = "No solution!";
      }
    }

    if (ch == 'c' || ch == 'C') {
      showErrors = !showErrors;
      msg = showErrors ? "Error highlighting ON" : "Error highlighting OFF";
    }

    if (ch == 'g' || ch == 'G') {
      sudoku.generate(35);
      board = sudoku.getPlayerBoard();
      solved = false;
      msg = "new Board generated.";
    }
  }

  endwin();
  return 0;
}
