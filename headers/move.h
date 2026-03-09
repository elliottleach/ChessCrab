#pragma once
#include "types.h"
#include <string>

enum class MoveFlag {
    Normal,             
    Capture,       
    DoublePush,
    EnPassant,
    CastleKing,
    CastleQueen,
    Promotion,
    PromotionCapture
};

struct Move {
    Square from;
    Square to;
    MoveFlag flag;
    PieceType promotionPiece; // only meaningful if flag is Promotion

    // default constructor - represents a null/invalid move
    Move() 
        : from(NO_SQUARE), to(NO_SQUARE), 
          flag(MoveFlag::Normal), promotionPiece(PieceType::NO_PIECE_TYPE) {}

    // normal constructor
    Move(Square from, Square to, MoveFlag flag = MoveFlag::Normal,
         PieceType promo = PieceType::NO_PIECE_TYPE)
        : from(from), to(to), flag(flag), promotionPiece(promo) {}

    // is this a valid move or a null placeholder?
    bool isNull() const {
        return from == NO_SQUARE;
    }

    // converts to UCI format e.g. "e2e4", "e7e8q"
    std::string toUCI() const;
};