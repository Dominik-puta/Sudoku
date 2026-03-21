#include "SudokuBoard.h"
#include <algorithm>
#include <random>
#include <vector>

SudokuBoard::SudokuBoard() {
  // Initialize empty board
  for (auto &r : playerBoard)
    r.fill(0);
}

bool SudokuBoard::isSafe(int row, int col, int num) const {
  int mask = 1 << num;
  return (rowUsed[row] & mask) == 0 && (colUsed[col] & mask) == 0 &&
         (boxUsed[getBoxIndex(row, col)] & mask) == 0;
}

bool SudokuBoard::isMoveValid(int row, int col, int num) {
  // If the user types the same number already in the cell, it's valid.
  if (playerBoard[row][col] == num)
    return true;

  // 1. Get the number currently sitting in this cell
  int currentNum = playerBoard[row][col];

  // 2. TEMPORARILY remove that number's bit so it doesn't block the new number
  if (currentNum != 0) {
    updateBitmasks(row, col, currentNum, false);
  }

  // 3. Now check if the NEW number is safe
  bool safe = isSafe(row, col, num);

  // 4. CRITICAL: Put the old number's bit back!
  // We are only 'asking', not 'placing' yet.
  if (currentNum != 0) {
    updateBitmasks(row, col, currentNum, true);
  }

  return safe;
}
void SudokuBoard::placePlayerNumber(int row, int col, int num) {

  // Remove whatever was there from bitmasks first
  int oldNum = playerBoard[row][col];
  if (oldNum >= 1 && oldNum <= 9) {
    updateBitmasks(row, col, oldNum, false);
  }

  // Update the actual board
  playerBoard[row][col] = num;

  // Add the new number to bitmasks
  if (num >= 1 && num <= 9) {
    updateBitmasks(row, col, num, true);
  }
}

void SudokuBoard::updateBitmasks(int row, int col, int num, bool add) {
  int mask = 1 << num;
  if (add) {
    rowUsed[row] |= mask;
    colUsed[col] |= mask;
    boxUsed[getBoxIndex(row, col)] |= mask;
  } else {
    rowUsed[row] &= ~mask;
    colUsed[col] &= ~mask;
    boxUsed[getBoxIndex(row, col)] &= ~mask;
  }
}

bool SudokuBoard::solveRecursive(int row, int col) {
  if (col == 9) {
    row++;
    col = 0;
  }
  if (row == 9)
    return true; // fully solved

  // Skip cells that are already filled in the solution
  if (solutionBoard[row][col] != 0) {
    return solveRecursive(row, col + 1);
  }

  for (int num = 1; num <= 9; ++num) {
    if (isSafe(row, col, num)) {
      solutionBoard[row][col] = num;
      updateBitmasks(row, col, num, true);

      if (solveRecursive(row, col + 1)) {
        return true;
      }

      // Backtrack
      solutionBoard[row][col] = 0;
      updateBitmasks(row, col, num, false);
    }
  }
  return false;
}

bool SudokuBoard::solve() {
  // Reset bitmasks
  rowUsed.fill(0);
  colUsed.fill(0);
  boxUsed.fill(0);

  // Initialize bitmasks from current board
  for (int i = 0; i < 9; ++i) {
    for (int j = 0; j < 9; ++j) {
      if (solutionBoard[i][j] != 0) {
        updateBitmasks(i, j, solutionBoard[i][j], true);
      }
    }
  }
  return solveRecursive(0, 0);
}

void SudokuBoard::generate(int difficulty) {
  // 1. Reset everything
  rowUsed.fill(0);
  colUsed.fill(0);
  boxUsed.fill(0);
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      solutionBoard[i][j] = 0;
      playerBoard[i][j] = 0;
      isFixed[i][j] = false;
    }
  }

  // 2. Fill the three diagonal 3x3 boxes randomly
  // This is mathematically guaranteed to be solvable!
  for (int box = 0; box < 9; box += 3) {
    std::vector<int> nums = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::shuffle(nums.begin(), nums.end(),
                 std::mt19937(std::random_device()()));

    int idx = 0;
    for (int r = 0; r < 3; r++) {
      for (int c = 0; c < 3; c++) {
        int val = nums[idx++];
        solutionBoard[box + r][box + c] = val;
        updateBitmasks(box + r, box + c, val, true);
      }
    }
  }

  // 3. Solve the rest (this will now be near-instant and 100% successful)
  solveRecursive(0, 0);

  // 4. Scramble the digits (The relabeling trick)
  std::vector<int> map_vec = {1, 2, 3, 4, 5, 6, 7, 8, 9};
  std::shuffle(map_vec.begin(), map_vec.end(),
               std::mt19937(std::random_device()()));
  int digit_map[10];
  for (int i = 0; i < 9; i++)
    digit_map[i + 1] = map_vec[i];

  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      solutionBoard[i][j] = digit_map[solutionBoard[i][j]];
    }
  }

  // 5. Create player board by removing numbers
  playerBoard = solutionBoard;
  int toRemove = 81 - difficulty;
  int removed = 0;
  while (removed < toRemove) {
    int r = rand() % 9;
    int c = rand() % 9;
    if (playerBoard[r][c] != 0) {
      playerBoard[r][c] = 0;
      removed++;
    }
  }

  // 6. Final Sync
  rowUsed.fill(0);
  colUsed.fill(0);
  boxUsed.fill(0);
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      if (playerBoard[i][j] != 0) {
        isFixed[i][j] = true;
        updateBitmasks(i, j, playerBoard[i][j], true);
      }
    }
  }
}
