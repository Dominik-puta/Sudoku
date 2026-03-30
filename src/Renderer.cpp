#include "Renderer.h"

namespace Renderer {
void initColors() {
  init_pair(1, COLOR_WHITE, COLOR_BLACK);  // empty
  init_pair(2, COLOR_CYAN, COLOR_BLACK);   // fixed e
  init_pair(3, COLOR_GREEN, COLOR_BLACK);  // user
  init_pair(4, COLOR_YELLOW, COLOR_BLACK); // cursor
  init_pair(5, COLOR_RED, COLOR_BLACK);    // errors
}
void draw(SudokuBoard &sudoku, int cy, int cx, const std::string &msg,
          bool showErrors, bool solved) {
  auto &board = sudoku.getPlayerBoard();
  clear();

  mvprintw(1, 21, "=== SUDOKU SOLVER ===");

  // Draw Cells
  for (int i = 0; i < 9; ++i) {
    for (int j = 0; j < 9; ++j) {
      int y = 4 + i + (i / 3);
      int x = 18 + j * 3;
      int color = 1;

      if (sudoku.isCellFixed(i, j))
        color = 2;
      else if (showErrors && !sudoku.isCorrect(i, j))
        color = 5;
      else if (board[i][j] != 0)
        color = 3;

      if (i == cy && j == cx)
        color = 4;

      attron(COLOR_PAIR(color));
      if (i == cy && j == cx)
        attron(A_REVERSE);
      mvprintw(y, x, "%c", board[i][j] == 0 ? '.' : '0' + board[i][j]);
      attroff(A_REVERSE | COLOR_PAIR(color));
    }
  }

  // Draw Borders
  for (int b = 0; b <= 3; ++b) {
    mvprintw(3 + b * 4, 17, "|--------|--------|--------|");
    int x = 17 + b * 9;
    for (int i = 0; i < 13; ++i)
      mvprintw(3 + i, x, "|");
  }

  // Status & Instructions
  mvprintw(17, 8, "%s", msg.c_str());
  mvprintw(19, 8,
           "Arrows Move  |  1-9 Fill  |  0 delete  |  s Solve  |  g Generate "
           " |  q Quit | V visualize ");
  mvprintw(21, 8, "c Toggle Errors (currently %s) | Solved: %s | Steps: %llu",
           showErrors ? "ON" : "OFF", solved ? "YES" : "NO",
           sudoku.getNodeCount());

  if (solved) {
    attron(A_BOLD | COLOR_PAIR(3));
    mvprintw(24, 23, "PUZZLE SOLVED!");
    attroff(A_BOLD | COLOR_PAIR(3));
  }

  refresh();
}
} // namespace Renderer
