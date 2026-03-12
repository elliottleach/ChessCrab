#include "movegen.h"
#include "board.h"
#include "move.h"
#include <vector>
#include "types.h"

static bool inBounds(int rank, int file) {
    return rank >= 0 && rank < 8 && file >= 0 && file < 8;
}

// Knights
void generateKnightMoves(const Board& board, int rank, int file, std::vector<Move>& moves) {
    
    int knightMoves[8][2] = {
        {2,1}, {2,-1}, {-2,1}, {-2,-1},
        {1,2}, {1,-2}, {-1,2}, {-1,-2}
    };
    int square = rank * 8 + file;
    for (auto& m : knightMoves) {
        int r = rank + m[0];
        int f = file + m[1];
        if (inBounds(r,f)) {
            Piece target = board.board[r][f];
            if (target.type == PieceType::NO_PIECE_TYPE || target.colour != board.sideToMove) {
                Move move(rank*8 + file, r*8 + f);
                if (target.type != PieceType::NO_PIECE_TYPE) {
                    move.flag = MoveFlag::Capture;
                    move.capturedPiece = target;
                }
                moves.push_back(move);
            }
        }
    }
}

// Sliding pieces (Rooks, Bishops, Queens)
void generateSlidingMoves(const Board& board, int rank, int file, bool straight, bool diagonal, std::vector<Move>& moves) {
    int straightDirs[4][2] = {{1,0},{-1,0},{0,1},{0,-1}};
    int diagonalDirs[4][2] = {{1,1},{1,-1},{-1,1},{-1,-1}};
    int square = rank * 8 + file;

    if (straight) {
        for (auto& d : straightDirs) {
            int r = rank + d[0];
            int f = file  + d[1];
            while (inBounds(r,f)) {
                Piece target = board.board[r][f];
                if (target.type == PieceType::NO_PIECE_TYPE) {
                    moves.emplace_back(square, r*8 + f);
                } else {
                    if (target.colour != board.sideToMove) {
                        Move move(square, r*8 + f, MoveFlag::Capture);
                        move.capturedPiece = target;
                        moves.push_back(move);
                    }
                    break; // stop sliding in this direction
                }
                r += d[0];
                f += d[1];
            }
        }
    }

    if (diagonal) {
        for (auto& d : diagonalDirs) {
            int r = rank + d[0];
            int f = file  + d[1];
            while (inBounds(r,f)) {
                Piece target = board.board[r][f];
                if (target.type == PieceType::NO_PIECE_TYPE) {
                    moves.emplace_back(square, r*8 + f);
                } else {
                    if (target.colour != board.sideToMove) {
                        Move move(square, r*8 + f, MoveFlag::Capture);
                        move.capturedPiece = target;
                        moves.push_back(move);
                    }
                    break; // stop sliding in this direction
                }
                r += d[0];
                f += d[1];
            }
        }
    }
}

static void generateKingMoves(const Board& board, int rank, int file, std::vector<Move>& moves) {
    int kingMoves[8][2] = {
        {1,0}, {-1,0}, {0,1}, {0,-1},
        {1,1}, {1,-1}, {-1,1}, {-1,-1}
    };
    int square = rank * 8 + file;
    for (auto& m : kingMoves) {
        int r = rank + m[0];
        int f = file + m[1];
        if (inBounds(r,f)) {
            Piece target = board.board[r][f];
            if (target.type == PieceType::NO_PIECE_TYPE || target.colour != board.sideToMove) {
                Move move(square, r*8 + f);
                if (target.type != PieceType::NO_PIECE_TYPE) {
                    move.flag = MoveFlag::Capture;
                    move.capturedPiece = target;
                }
                moves.push_back(move);
            }
        }
    }

    // Castling
    if (!board.isInCheck()) {
        if (board.canCastleKingside()) {
            if (board.board[rank][5].type == PieceType::NO_PIECE_TYPE &&
                board.board[rank][6].type == PieceType::NO_PIECE_TYPE) {
                Board copy = board;
                copy.makeMove(Move(square, rank * 8 + 5));
                copy.sideToMove = board.sideToMove;
                if (!copy.isInCheck()) {
                    moves.push_back(Move(square, rank * 8 + 6, MoveFlag::CastleKing));
                }
            }
        }
        if (board.canCastleQueenside()) {
            if (board.board[rank][3].type == PieceType::NO_PIECE_TYPE &&
                board.board[rank][2].type == PieceType::NO_PIECE_TYPE &&
                board.board[rank][1].type == PieceType::NO_PIECE_TYPE) {
                Board copy = board;
                copy.makeMove(Move(square, rank * 8 + 3));
                copy.sideToMove = board.sideToMove;
                if (!copy.isInCheck()) {
                    moves.push_back(Move(square, rank * 8 + 2, MoveFlag::CastleQueen));
                }
            }
        }
    }
}
// Pawns
static void generatePawnMoves(const Board& board, int rank, int file, std::vector<Move>& moves) {
    int square       = rank * 8 + file;
    int direction    = (board.sideToMove == Colour::White) ? 1 : -1;
    int startRank    = (board.sideToMove == Colour::White) ? 1 : 6;
    int promoRank    = (board.sideToMove == Colour::White) ? 7 : 0;
    int pushRank     = rank + direction;       
    int captureRank  = rank + direction;       
//single 
    if (inBounds(pushRank, file) &&
        board.board[pushRank][file].type == PieceType::NO_PIECE_TYPE) {

        if (pushRank == promoRank) {
            for (PieceType promo : {PieceType::Queen, PieceType::Rook,
                                    PieceType::Bishop, PieceType::Knight}) {
                moves.emplace_back(square, pushRank * 8 + file, MoveFlag::Promotion, promo);
            }
        } else {
            moves.emplace_back(square, pushRank * 8 + file, MoveFlag::Normal);
        }

        if (rank == startRank) {
            int doublePushRank = rank + direction * 2;
            if (inBounds(doublePushRank, file) &&
                board.board[doublePushRank][file].type == PieceType::NO_PIECE_TYPE) {
                moves.emplace_back(square, doublePushRank * 8 + file, MoveFlag::DoublePush);
            }
        }
    }

    for (int df : {-1, 1}) {
        int f = file + df;
        if (!inBounds(captureRank, f)) continue;

        Piece target = board.board[captureRank][f];
        int toSq = captureRank * 8 + f;

        if (target.type != PieceType::NO_PIECE_TYPE &&
            target.colour != board.sideToMove) {

            if (captureRank == promoRank) {
                for (PieceType promo : {PieceType::Queen, PieceType::Rook,
                                        PieceType::Bishop, PieceType::Knight}) {
                    Move m(square, toSq, MoveFlag::PromotionCapture, promo);
                    m.capturedPiece = target;
                    moves.push_back(m);
                }
            } else {
                Move m(square, toSq, MoveFlag::Capture);
                m.capturedPiece = target;
                moves.push_back(m);
            }
        }

// en passant
        if (board.enPassantSquare != NO_SQUARE && toSq == board.enPassantSquare) {
            Colour enemyColour = (board.sideToMove == Colour::White)
                                 ? Colour::Black : Colour::White;
            Move m(square, toSq, MoveFlag::EnPassant);
            m.capturedPiece = { PieceType::Pawn, enemyColour };
            moves.push_back(m);
        }
    }
}

std::vector<Move> MoveGen::generateLegalMoves(const Board& board) {
    std::vector<Move> moves;
    for (int rank = 0; rank < 8; ++rank) {
        for (int file = 0; file < 8; ++file) {
            Piece piece = board.board[rank][file];
            if (piece.type != PieceType::NO_PIECE_TYPE && piece.colour == board.sideToMove) {
                switch (piece.type) {
                    case PieceType::Pawn:
                        generatePawnMoves(board, rank, file, moves);
                        break;
                    case PieceType::Knight:
                        generateKnightMoves(board, rank, file, moves);
                        break;
                    case PieceType::Bishop:
                        generateSlidingMoves(board, rank, file, false, true, moves);
                        break;
                    case PieceType::Rook:
                        generateSlidingMoves(board, rank, file, true, false, moves);
                        break;
                    case PieceType::Queen:
                        generateSlidingMoves(board, rank, file, true, true, moves);
                        break;
                    case PieceType::King:
                        generateKingMoves(board, rank, file, moves);
                        break;
                    default:
                        break;
                }
            }
        }
    }

    std::vector<Move> legalMoves;
    for (const Move& move : moves) {
        Board copy = board;
        copy.makeMove(move);
        if (!copy.isInCheck()) {
            legalMoves.push_back(move);
        }
    }
    return legalMoves;
}