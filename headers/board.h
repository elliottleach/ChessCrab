#pragma once
#include "types.h"
#include "move.h"
#include <vector>
#include <string>

struct Board {
    Piece board[8][8]; // 8x8 board representation using piece codes    
    Colour sideToMove;

    bool castleWhiteKing;
    bool castleWhiteQueen;
    bool castleBlackKing;
    bool castleBlackQueen;

    std::vector<Move> moveHistory; // history of moves made on this board
    Square enPassantSquare; // set to NO_SQUARE if not available

    Board(); // constructor to initialize the board to the starting position

    // generates all legal moves for the current position
    std::vector<Move> generateLegalMoves() const;

    // applies a move to the board, updating the position accordingly
    void makeMove(const Move& move);

    // undoes the last move made, restoring the previous position
    void undoMove(const Move& move);

    bool canCastleKingside() const; // returns true if the current player can castle kingside
    bool canCastleQueenside() const; // returns true if the current player can castle queens


    bool isInCheck() const;
    bool isCheckmate() const;
    bool isStalemate() const;

    // returns a string representation of the board (for debugging)
    std::string toString() const;    
};

