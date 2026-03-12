#include "board.h"
#include "types.h"
#include "move.h"
#include "movegen.h"
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
    
    move_copy.savedState = {
        castleWhiteKing, castleWhiteQueen,
        castleBlackKing, castleBlackQueen,
        enPassantSquare
    };

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
    sideToMove = (sideToMove == Colour::White) ? Colour::Black : Colour::White;
}

void Board::undoMove() {
    if (moveHistory.empty()) {
        std::cerr << "Error: No moves to undo!" << std::endl;
        return;
    }
    Move lastMove = moveHistory.back();
    moveHistory.pop_back();

    castleWhiteKing  = lastMove.savedState.castleWK;
    castleWhiteQueen = lastMove.savedState.castleWQ;
    castleBlackKing  = lastMove.savedState.castleBK;
    castleBlackQueen = lastMove.savedState.castleBQ;
    enPassantSquare  = lastMove.savedState.enPassantSquare;
    board[lastMove.from / 8][lastMove.from % 8] = board[lastMove.to / 8][lastMove.to % 8];
    board[lastMove.to / 8][lastMove.to % 8] = lastMove.capturedPiece;
    sideToMove = (sideToMove == Colour::White) ? Colour::Black : Colour::White;
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

bool Board::isInCheck() const {
    // step 1 - find the king's square
    int kingRank = -1, kingFile = -1;
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            Piece p = board[rank][file];
            if (p.type == PieceType::King && p.colour == sideToMove) {
                kingRank = rank;
                kingFile = file;
            }
        }
    }

    // step 2 - is any enemy piece attacking that square?
    Colour enemy = (sideToMove == Colour::White) ? Colour::Black : Colour::White;

    // check enemy knights
    int knightMoves[8][2] = {
        {2,1},{2,-1},{-2,1},{-2,-1},
        {1,2},{1,-2},{-1,2},{-1,-2}
    };
    for (auto& km : knightMoves) {
        int r = kingRank + km[0];
        int f = kingFile  + km[1];
        if (r >= 0 && r < 8 && f >= 0 && f < 8) {
            Piece p = board[r][f];
            if (p.type == PieceType::Knight && p.colour == enemy) return true;
        }
    }

    // check enemy pawns
    int pawnDir = (sideToMove == Colour::White) ? 1 : -1;
    for (int df : {-1, 1}) {
        int r = kingRank + pawnDir;
        int f = kingFile + df;
        if (r >= 0 && r < 8 && f >= 0 && f < 8) {
            Piece p = board[r][f];
            if (p.type == PieceType::Pawn && p.colour == enemy) return true;
        }
    }

    // check enemy rooks and queens (straight lines)
    int straightDirs[4][2] = {{1,0},{-1,0},{0,1},{0,-1}};
    for (auto& d : straightDirs) {
        int r = kingRank + d[0];
        int f = kingFile  + d[1];
        while (r >= 0 && r < 8 && f >= 0 && f < 8) {
            Piece p = board[r][f];
            if (p.type != PieceType::NO_PIECE_TYPE) {
                if (p.colour == enemy &&
                   (p.type == PieceType::Rook || p.type == PieceType::Queen))
                    return true;
                break; // blocked by any piece
            }
            r += d[0]; f += d[1];
        }
    }

    // check enemy bishops and queens (diagonals)
    int diagDirs[4][2] = {{1,1},{1,-1},{-1,1},{-1,-1}};
    for (auto& d : diagDirs) {
        int r = kingRank + d[0];
        int f = kingFile  + d[1];
        while (r >= 0 && r < 8 && f >= 0 && f < 8) {
            Piece p = board[r][f];
            if (p.type != PieceType::NO_PIECE_TYPE) {
                if (p.colour == enemy &&
                   (p.type == PieceType::Bishop || p.type == PieceType::Queen))
                    return true;
                break;
            }
            r += d[0]; f += d[1];
        }
    }

    // check enemy king (prevents kings from touching)
    int kingDirs[8][2] = {
        {1,0},{-1,0},{0,1},{0,-1},
        {1,1},{1,-1},{-1,1},{-1,-1}
    };
    for (auto& d : kingDirs) {
        int r = kingRank + d[0];
        int f = kingFile  + d[1];
        if (r >= 0 && r < 8 && f >= 0 && f < 8) {
            Piece p = board[r][f];
            if (p.type == PieceType::King && p.colour == enemy) return true;
        }
    }

    return false;
}

bool Board::isCheckmate() const {
    return false; // placeholder implementation
}


std::vector<Move> Board::generateLegalMoves() const {
    return MoveGen::generateLegalMoves(*this);
}