#pragma once

enum class PieceType {
    Pawn,
    Knight,
    Bishop,
    Rook,
    Queen,
    King,
    NO_PIECE_TYPE
};

enum class Colour {
    White,
    Black,
    NO_COLOUR
};

struct Piece {
    PieceType type;
    Colour colour;
};

const Piece NO_PIECE = {PieceType::NO_PIECE_TYPE, Colour::NO_COLOUR};


using Square = int;
const Square NO_SQUARE = -1;

const int BOARD_SIZE = 64;

// https://en.wikipedia.org/wiki/Chess_piece_relative_value
const int piece_values[] = {
    100, // Pawn
    320, // Knight
    330, // Bishop
    500, // Rook
    900, // Queen
    0, // King
    0 // NO_PIECE_TYPE
};