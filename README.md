# ChessCrab
ChessCrab is a custom chess engine written in C++, built as a personal project to deepen my understanding of systems programming, algorithms, and game creation.

This project started from a simple goal: to learn C++. Chess felt like the right challenge because it combines data structures, recursion, optimisation, and strategic reasoning in one system, and is also a really cool game!

Motivation

I created ChessCrab to move beyond high-level languages like Python and gain hands-on experience with:
- Lower-level memory management
- Performance optimisation
- Object-oriented design in C++
- Algorithmic thinking in a real-world system

Rather than following a tutorial step-by-step, I wanted to understand how and why a chess engine works (and compare it to ones such as Stockfish) — from board representation all the way to decision-making.

Features
- Full chess board representation
- Legal move generation for all pieces
- Game state evaluation (basic heuristic scoring)


How It Was Built
1. Board Representation

The first challenge was deciding how to represent the chess board.

I explored different approaches (2D arrays vs more advanced structures) and settled on 2D array as it balanced:
- Simplicity (easy debugging)
- Efficiency (fast access and updates)
- 
But also allows extensibility further on into the project for increasing efficiency.

This step was crucial because everything else depends on it.

2. Move Generation

Next, I implemented move generation for each piece:
- Pawns (including special rules like promotion/en passant implemented)
- Sliding pieces (rook, bishop, queen)
- Knight and king movement

This required careful handling of:
- Board boundaries
- Captures vs empty squares
- Valid vs illegal moves (e.g. moving into check)

3. Game Logic
- Once moves were working, I added:
- Turn management
- Check detection
- Game-ending conditions (checkmate/stalemate)

This was one of the trickier parts, as it required simulating moves and verifying resulting states.

4. Iteration & Debugging
A large part of the project involved:
- Debugging edge cases (especially illegal moves)
- Refactoring for a cleaner structure


Tech Stack
- Language: C++
- Paradigm: Object-Oriented Programming
- CMake

What I Learned
This project significantly improved my understanding of:
- Writing performant code in C++
- Designing larger systems from scratch
- Translating real-world rules into code
- Debugging complex logic
- Utilising AI to ...

