#pragma once
#include <array>
#include <functional>

class SudokuBoard {
public:
  using Board = std::array<std::array<int, 9>, 9>;
  bool visualize = false;

  SudokuBoard();
  bool solve(bool visualize = false, std::function<void()> onStep = nullptr);
  void generate(int difficulty);
  Board &getPlayerBoard() { return playerBoard; }
  const Board &getPlayerBoard() const { return playerBoard; }
  bool isCellFixed(int row, int col) const { return isFixed[row][col]; }
  void revealSolution() { playerBoard = solutionBoard; }
  void placePlayerNumber(int row, int col, int num);
  bool isSafe(int row, int col, int num) const;
  bool isMoveValid(int row, int col, int num);
  int getSolutionValue(int row, int col) const {
    return solutionBoard[row][col];
  }
  bool isCorrect(int row, int col) const {
    // If cell is empty, it's not "incorrect" yet
    if (playerBoard[row][col] == 0)
      return true;
    return playerBoard[row][col] == solutionBoard[row][col];
  }
  unsigned long long getNodeCount() const { return nodeCount; }

private:
  Board playerBoard{};
  Board solutionBoard{};
  std::array<std::array<bool, 9>, 9> isFixed{}; // Tracks fixed cells
  unsigned long long nodeCount = 0;

  // Bitmasks for fast checking
  std::array<int, 9> rowUsed{};
  std::array<int, 9> colUsed{};
  std::array<int, 9> boxUsed{};

  bool solveRecursive(int row, int col, bool visualize,
                      std::function<void()> onStep);
  void updateBitmasks(int row, int col, int num, bool add);

  int getBoxIndex(int row, int col) const { return (row / 3) * 3 + (col / 3); }
};
