cmake_minimum_required(VERSION 3.10)
project(SudokuGame)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Handle Ncurses dependency
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIRS})

# 2. Include the 'include' folder so SudokuBoard.h can be found
include_directories(include)

# 3. Define the source files
set(SOURCES 
    src/main.cpp 
    src/SudokuBoard.cpp
)

# 4. Create the executable
add_executable(sudoku ${SOURCES})

# 5. Link Ncurses libraries
target_link_libraries(sudoku ${CURSES_LIBRARIES})
