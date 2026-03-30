#include "Renderer.h"
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

  Renderer::initColors();

  SudokuBoard sudoku;
  sudoku.generate(10);
  auto &board = sudoku.getPlayerBoard();

  int cy = 0, cx = 0;
  std::string msg = "";

  bool solved = false;
  bool showErrors = false;

  while (true) {

    Renderer::draw(sudoku, cy, cx, msg, showErrors, solved);

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
        sudoku.placePlayerNumber(cy, cx, val);
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
        msg = "Solved successfully in " +
              std::to_string(sudoku.getNodeCount()) + " steps.";
      } else {
        msg = "No solution!";
      }
    }
    if (ch == 'v' || ch == 'V') {
      msg = "Visualizing solve...";
      sudoku.solve(true, [&]() {
        Renderer::draw(sudoku, cy, cx, msg, showErrors, solved);
      });
      msg = "Finished animation.";
    }

    if (ch == 'c' || ch == 'C') {
      showErrors = !showErrors;
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
