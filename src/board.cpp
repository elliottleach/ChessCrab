#include "board.h"
#include "types.h"
#include "move.h"
#include <iostream>

Board::Board() {
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            board[rank][file] = NO_PIECE;
        }
    }

    // step 2 — place white pieces on rank 0 (row 1)
    board[0][0] = { PieceType::Rook,   Colour::White };
    board[0][1] = { PieceType::Knight, Colour::White };
    board[0][2] = { PieceType::Bishop, Colour::White };
    board[0][3] = { PieceType::Queen,  Colour::White };
    board[0][4] = { PieceType::King,   Colour::White };
    board[0][5] = { PieceType::Bishop, Colour::White };
    board[0][6] = { PieceType::Knight, Colour::White };
    board[0][7] = { PieceType::Rook,   Colour::White };

    // step 3 — white pawns on rank 1 (row 2)
    for (int file = 0; file < 8; file++) {
        board[1][file] = { PieceType::Pawn, Colour::White };
    }

    // step 4 — black pawns on rank 6 (row 7)
    for (int file = 0; file < 8; file++) {
        board[6][file] = { PieceType::Pawn, Colour::Black };
    }

    // step 5 — black pieces on rank 7 (row 8)
    board[7][0] = { PieceType::Rook,   Colour::Black };
    board[7][1] = { PieceType::Knight, Colour::Black };
    board[7][2] = { PieceType::Bishop, Colour::Black };
    board[7][3] = { PieceType::Queen,  Colour::Black };
    board[7][4] = { PieceType::King,   Colour::Black };
    board[7][5] = { PieceType::Bishop, Colour::Black };
    board[7][6] = { PieceType::Knight, Colour::Black };
    board[7][7] = { PieceType::Rook,   Colour::Black };

    // step 6 — game state
    sideToMove       = Colour::White;
    enPassantSquare  = NO_SQUARE;
    castleWhiteKing  = true;
    castleWhiteQueen = true;
    castleBlackKing  = true;
    castleBlackQueen = true;
}

void Board::makeMove(const Move& move) {
    Piece place_holder;

    Move move_copy = move; // create a copy of the move to modify
    move_copy.capturedPiece = board[move.to / 8][move.to % 8];
    
    if (move.from != NO_SQUARE) {
        int rank = move.from / 8;
        int file = move.from % 8;
        place_holder = board[rank][file];
    } else {
        place_holder = NO_PIECE;
    }

    board[move.to / 8][move.to % 8] = place_holder;
    if (move.from != NO_SQUARE) {
        board[move.from / 8][move.from % 8] = NO_PIECE;
    }
    moveHistory.push_back(move_copy);
}

void Board::undoMove() {
    if (moveHistory.empty()) {
        std::cerr << "Error: No moves to undo!" << std::endl;
        return;
    }
    Move lastMove = moveHistory.back();
    moveHistory.pop_back();
    
    board[lastMove.from / 8][lastMove.from % 8] = board[lastMove.to / 8][lastMove.to % 8];
    board[lastMove.to / 8][lastMove.to % 8] = lastMove.capturedPiece;
}

bool Board::canCastleKingside() const {
    if (sideToMove == Colour::White) {
        return castleWhiteKing;
    } else {
        return castleBlackKing;
    }
}

bool Board::canCastleQueenside() const {
    if (sideToMove == Colour::White) {
        return castleWhiteQueen;
    } else {
        return castleBlackQueen;
    }
}

bool Board::isInCheck() const {
    return false; // placeholder implementation
}

std::string Board::toString() const {
    std::string result;
    for (int rank = 7; rank >= 0; --rank) {
        for (int file = 0; file < 8; ++file) {
            const Piece& piece = board[rank][file];
            if (piece.type == PieceType::NO_PIECE_TYPE) {
                result += '.';
                result += ' ';
            } else {
                char c;
                switch (piece.type) {
                    case PieceType::Pawn: c = 'p'; break;
                    case PieceType::Knight: c = 'n'; break;
                    case PieceType::Bishop: c = 'b'; break;
                    case PieceType::Rook: c = 'r'; break;
                    case PieceType::Queen: c = 'q'; break;
                    case PieceType::King: c = 'k'; break;
                    default: c = '?'; break;
                }
                if (piece.colour == Colour::White) {
                    c = toupper(c);
                }
                result += c;
                result += ' ';
            }
        }
        result += '\n';
    }
    return result;
}