# Heracles

Heracles is a chess engine written in C that uses the Minimax algorithm with a bitboard approach. This university project aims to provide a robust and efficient implementation of a chess engine that can evaluate and play chess positions.

## Table of Contents

- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
- [Algorithm](#algorithm)
- [Contributions](#contributions)
- [Acknowledgements](#acknowledgements)

## Features

- **Minimax Algorithm**: Implements the Minimax algorithm with alpha-beta pruning.
- **Bitboard Representation**: Utilizes bitboards for efficient board representation and move generation.
- **Move Generation**: Efficient move generation for all pieces.
- **Evaluation Function**: Basic evaluation function to assess board positions.
- **Command-Line Interface**: Simple CLI for interacting with the engine.

## Installation

### Prerequisites

- GCC (GNU Compiler Collection)
- Make (there is no makefile but the implicit rules of make are enough)

### Steps

1. Clone the repository:
    ```sh
    git clone https://github.com/loiclgrx/heracles.git
    cd heracles
    ```

2. Compile the project:
    ```sh
    make
    ```

### Visual Studio

This repository includes a Visual Studio project file, allowing you to open Heracles in Visual Studio and build it using the integrated build system.

## Usage

After compiling, you can run the chess engine from the command line:

```sh
./heracles
```

### Command-Line Interface

- **newgame**: Starts a new game.
- **move <move>**: Makes a move (e.g., `move e2e4`).
- **display**: Displays the current board.
- **quit**: Exits the program.

Example session:
```sh
./heracles
> newgame
> display
  a b c d e f g h
8 r n b q k b n r 8
7 p p p p p p p p 7
6 . . . . . . . . 6
5 . . . . . . . . 5
4 . . . . . . . . 4
3 . . . . . . . . 3
2 P P P P P P P P 2
1 R N B Q K B N R 1
  a b c d e f g h
> move e2e4
> display
...
> quit
```

### Chess GUIs

Since Heracles follows the Universal Chess Interface (UCI) protocol, the executable you get after building the project can be used in chess graphical user interfaces such as Arena.

## Algorithm

Heracles uses the Minimax algorithm with alpha-beta pruning for searching the game tree. The bitboard approach is used to represent the chess board, which allows for efficient computation of moves and board evaluations.

### Minimax with Alpha-Beta Pruning

The Minimax algorithm is a recursive method used for decision-making in game theory and artificial intelligence. Alpha-beta pruning is an optimization technique for the Minimax algorithm that reduces the number of nodes evaluated in the search tree.

### Bitboard Representation

A bitboard is a 64-bit integer used to represent the positions of pieces on a chessboard. Each bit corresponds to a square on the board, allowing for efficient manipulation and move generation.

## Contributions

This project was part of a course from the engineering school IMT Nord Europe which I took along two classmates :
- [Julian Hutin](https://www.linkedin.com/in/julian-h-03708219b/)
- [Hamza Lasri](https://www.linkedin.com/in/hamza-lasri/)

## Acknowledgements

- [Chess Programming Wiki](https://www.chessprogramming.org/Main_Page) for resources and algorithms related to chess programming.
- [The Bitboard Approach](https://www.chessprogramming.org/Bitboards) for efficient board representation techniques.
