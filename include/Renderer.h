#pragma once
#include "SudokuBoard.h"
#include <ncurses.h>
#include <string>

namespace Renderer {
void draw(SudokuBoard &sudoku, int cy, int cx, const std::string &msg,
          bool showErrors, bool solved);
void initColors();
} // namespace Renderer
