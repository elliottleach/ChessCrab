#pragma once
#include "board.h"
#include <vector>

namespace MoveGen {

std::vector<Move> generateLegalMoves(const Board& board);

}