#include "board.h"
#include "move.h"
#include "types.h"
#include <iostream>
#include <string>

// Square helpers: sq(rank, file) matches board[rank][file]
static constexpr Square sq(int rank, int file) { return rank * 8 + file; }

// ─── tiny test framework ───────────────────────────────────────────────────
static int passed = 0, failed = 0;

static void check(bool condition, const std::string& label) {
    if (condition) {
        std::cout << "  PASS  " << label << "\n";
        ++passed;
    } else {
        std::cout << "  FAIL  " << label << "\n";
        ++failed;
    }
}

static void section(const std::string& name) {
    std::cout << "\n=== " << name << " ===\n";
}

// ─── helpers ──────────────────────────────────────────────────────────────
static Piece pieceAt(const Board& b, int rank, int file) {
    return b.board[rank][file];
}

static bool isEmpty(const Board& b, int rank, int file) {
    return b.board[rank][file].type == PieceType::NO_PIECE_TYPE;
}

static bool isType(const Board& b, int rank, int file,
                   PieceType t, Colour c) {
    auto p = b.board[rank][file];
    return p.type == t && p.colour == c;
}

// ─── tests ────────────────────────────────────────────────────────────────

void test_normal_move() {
    section("Normal pawn push (e2→e3)");
    Board b;
    // White pawn starts at rank 1, file 4 (e2)
    Move m(sq(1,4), sq(2,4), MoveFlag::Normal);
    b.makeMove(m);

    check(isEmpty(b, 1, 4),                         "e2 is now empty");
    check(isType(b, 2, 4, PieceType::Pawn, Colour::White), "e3 has white pawn");
    check(b.sideToMove == Colour::Black,             "side flipped to Black");
    check(b.enPassantSquare == NO_SQUARE,            "no en passant set");

    b.undoMove();
    check(isType(b, 1, 4, PieceType::Pawn, Colour::White), "undo: pawn back on e2");
    check(isEmpty(b, 2, 4),                         "undo: e3 empty again");
    check(b.sideToMove == Colour::White,             "undo: side flipped back");
}

void test_double_push() {
    section("Double pawn push (e2→e4) — en passant square");
    Board b;
    Move m(sq(1,4), sq(3,4), MoveFlag::DoublePush);
    b.makeMove(m);

    check(isEmpty(b, 1, 4),                         "e2 is now empty");
    check(isType(b, 3, 4, PieceType::Pawn, Colour::White), "e4 has white pawn");
    check(b.enPassantSquare == sq(2,4),              "en passant target set to e3");

    b.undoMove();
    check(isType(b, 1, 4, PieceType::Pawn, Colour::White), "undo: pawn back on e2");
    check(isEmpty(b, 3, 4),                         "undo: e4 empty");
    check(b.enPassantSquare == NO_SQUARE,            "undo: en passant cleared");
}

void test_en_passant() {
    section("En passant capture (white e5 takes d5 pawn via d6)");
    Board b;
    // Clear board and set up manually
    for (int r = 0; r < 8; r++)
        for (int f = 0; f < 8; f++)
            b.board[r][f] = NO_PIECE;
    b.board[4][4] = { PieceType::Pawn, Colour::White }; // white pawn e5
    b.board[4][3] = { PieceType::Pawn, Colour::Black }; // black pawn d5
    b.enPassantSquare = sq(5, 3);                        // en passant target d6
    b.sideToMove = Colour::White;

    Move m(sq(4,4), sq(5,3), MoveFlag::EnPassant);
    b.makeMove(m);

    check(isEmpty(b, 4, 4),                         "e5 is now empty");
    check(isEmpty(b, 4, 3),                         "d5 captured pawn removed");
    check(isType(b, 5, 3, PieceType::Pawn, Colour::White), "white pawn on d6");

    b.undoMove();
    check(isType(b, 4, 4, PieceType::Pawn, Colour::White), "undo: white pawn back on e5");
    check(isType(b, 4, 3, PieceType::Pawn, Colour::Black), "undo: black pawn restored on d5");
    check(isEmpty(b, 5, 3),                         "undo: d6 empty");
    check(b.enPassantSquare == sq(5,3),              "undo: en passant square restored");
}

void test_capture() {
    section("Normal capture (white pawn takes on d5)");
    Board b;
    for (int r = 0; r < 8; r++)
        for (int f = 0; f < 8; f++)
            b.board[r][f] = NO_PIECE;
    b.board[4][4] = { PieceType::Pawn, Colour::White }; // e5
    b.board[5][3] = { PieceType::Knight, Colour::Black }; // d6 — target
    b.sideToMove = Colour::White;

    Move m(sq(4,4), sq(5,3), MoveFlag::Capture);
    b.makeMove(m);

    check(isEmpty(b, 4, 4),                              "e5 is now empty");
    check(isType(b, 5, 3, PieceType::Pawn, Colour::White), "white pawn on d6");

    b.undoMove();
    check(isType(b, 4, 4, PieceType::Pawn, Colour::White),   "undo: pawn back on e5");
    check(isType(b, 5, 3, PieceType::Knight, Colour::Black), "undo: black knight restored");
}

void test_castle_kingside() {
    section("Kingside castling (white O-O)");
    Board b;
    // Clear the f1 and g1 squares as they are in the starting position
    b.board[0][5] = NO_PIECE; // f1
    b.board[0][6] = NO_PIECE; // g1

    // King e1 (sq 4) → g1 (sq 6)
    Move m(sq(0,4), sq(0,6), MoveFlag::CastleKing);
    b.makeMove(m);

    check(isEmpty(b, 0, 4),                          "e1 (king origin) empty");
    check(isType(b, 0, 6, PieceType::King, Colour::White), "king on g1");
    check(isEmpty(b, 0, 7),                          "h1 (rook origin) empty");
    check(isType(b, 0, 5, PieceType::Rook, Colour::White), "rook on f1");
    check(!b.castleWhiteKing,                        "white kingside right revoked");
    check(!b.castleWhiteQueen,                       "white queenside right revoked");

    b.undoMove();
    check(isType(b, 0, 4, PieceType::King, Colour::White), "undo: king back on e1");
    check(isType(b, 0, 7, PieceType::Rook, Colour::White), "undo: rook back on h1");
    check(isEmpty(b, 0, 5),                          "undo: f1 empty");
    check(isEmpty(b, 0, 6),                          "undo: g1 empty");
    check(b.castleWhiteKing,                         "undo: kingside right restored");
}

void test_castle_queenside() {
    section("Queenside castling (white O-O-O)");
    Board b;
    b.board[0][1] = NO_PIECE; // b1
    b.board[0][2] = NO_PIECE; // c1
    b.board[0][3] = NO_PIECE; // d1

    // King e1 (sq 4) → c1 (sq 2)
    Move m(sq(0,4), sq(0,2), MoveFlag::CastleQueen);
    b.makeMove(m);

    check(isEmpty(b, 0, 4),                          "e1 (king origin) empty");
    check(isType(b, 0, 2, PieceType::King, Colour::White), "king on c1");
    check(isEmpty(b, 0, 0),                          "a1 (rook origin) empty");
    check(isType(b, 0, 3, PieceType::Rook, Colour::White), "rook on d1");

    b.undoMove();
    check(isType(b, 0, 4, PieceType::King, Colour::White), "undo: king back on e1");
    check(isType(b, 0, 0, PieceType::Rook, Colour::White), "undo: rook back on a1");
    check(isEmpty(b, 0, 2),                          "undo: c1 empty");
    check(isEmpty(b, 0, 3),                          "undo: d1 empty");
}

void test_promotion() {
    section("Pawn promotion (white e7→e8=Q)");
    Board b;
    for (int r = 0; r < 8; r++)
        for (int f = 0; f < 8; f++)
            b.board[r][f] = NO_PIECE;
    b.board[6][4] = { PieceType::Pawn, Colour::White }; // e7
    b.sideToMove = Colour::White;

    Move m(sq(6,4), sq(7,4), MoveFlag::Promotion, PieceType::Queen);
    b.makeMove(m);

    check(isEmpty(b, 6, 4),                           "e7 is now empty");
    check(isType(b, 7, 4, PieceType::Queen, Colour::White), "e8 has white queen");

    b.undoMove();
    check(isType(b, 6, 4, PieceType::Pawn, Colour::White), "undo: pawn restored on e7");
    check(isEmpty(b, 7, 4),                           "undo: e8 empty");
}

void test_promotion_capture() {
    section("Promotion capture (white e7 takes f8=R)");
    Board b;
    for (int r = 0; r < 8; r++)
        for (int f = 0; f < 8; f++)
            b.board[r][f] = NO_PIECE;
    b.board[6][4] = { PieceType::Pawn,   Colour::White }; // e7
    b.board[7][5] = { PieceType::Bishop, Colour::Black }; // f8 — target
    b.sideToMove = Colour::White;

    Move m(sq(6,4), sq(7,5), MoveFlag::PromotionCapture, PieceType::Rook);
    b.makeMove(m);

    check(isEmpty(b, 6, 4),                           "e7 is now empty");
    check(isType(b, 7, 5, PieceType::Rook, Colour::White), "f8 has white rook");

    b.undoMove();
    check(isType(b, 6, 4, PieceType::Pawn,   Colour::White), "undo: pawn back on e7");
    check(isType(b, 7, 5, PieceType::Bishop, Colour::Black), "undo: black bishop restored on f8");
}

void test_castling_rights_on_king_move() {
    section("Castling rights revoked when king moves");
    Board b;
    b.board[0][5] = NO_PIECE;
    b.board[0][6] = NO_PIECE;

    Move m(sq(0,4), sq(0,5), MoveFlag::Normal); // king slides to f1
    b.makeMove(m);

    check(!b.castleWhiteKing,  "white kingside right revoked after king move");
    check(!b.castleWhiteQueen, "white queenside right revoked after king move");
    check(b.castleBlackKing,   "black rights untouched");
    check(b.castleBlackQueen,  "black rights untouched");

    b.undoMove();
    check(b.castleWhiteKing,   "undo: white kingside right restored");
    check(b.castleWhiteQueen,  "undo: white queenside right restored");
}

void test_castling_rights_on_rook_move() {
    section("Castling rights revoked when rook moves");
    Board b;
    b.board[1][0] = NO_PIECE; // clear a2 so a1 rook can move
    b.board[0][1] = NO_PIECE; // clear b1 so rook has a square to go to (we'll just move rook to b1)

    // Actually move rook from a1 (sq 0) to a3 (sq 16)
    b.board[2][0] = NO_PIECE;
    Move m(sq(0,0), sq(2,0), MoveFlag::Normal);
    b.makeMove(m);

    check(!b.castleWhiteQueen, "white queenside right revoked after a1 rook move");
    check(b.castleWhiteKing,   "white kingside right untouched");

    b.undoMove();
    check(b.castleWhiteQueen,  "undo: white queenside right restored");
}

void test_castling_rights_on_rook_capture() {
    section("Castling rights revoked when enemy captures home rook");
    Board b;
    for (int r = 0; r < 8; r++)
        for (int f = 0; f < 8; f++)
            b.board[r][f] = NO_PIECE;
    b.board[7][7] = { PieceType::Rook,   Colour::Black }; // h8 rook
    b.board[0][7] = { PieceType::Rook,   Colour::White }; // h1 rook
    b.board[1][7] = { PieceType::Queen,  Colour::White }; // attacker — just for setup
    b.sideToMove  = Colour::Black;

    // Black queen captures white h1 rook
    b.board[7][6] = { PieceType::Queen, Colour::Black };
    Move m(sq(7,6), sq(0,7), MoveFlag::Capture);
    b.makeMove(m);

    check(!b.castleWhiteKing,  "white kingside right revoked after h1 rook captured");
    check(b.castleWhiteQueen,  "white queenside right unaffected");

    b.undoMove();
    check(b.castleWhiteKing,   "undo: white kingside right restored");
}

// ─── main ─────────────────────────────────────────────────────────────────

int main() {
    test_normal_move();
    test_double_push();
    test_en_passant();
    test_capture();
    test_castle_kingside();
    test_castle_queenside();
    test_promotion();
    test_promotion_capture();
    test_castling_rights_on_king_move();
    test_castling_rights_on_rook_move();
    test_castling_rights_on_rook_capture();

    std::cout << "\n──────────────────────────────\n";
    std::cout << "Results: " << passed << " passed, " << failed << " failed\n";
    return failed == 0 ? 0 : 1;
}
