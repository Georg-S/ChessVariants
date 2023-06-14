#include "Pieces/Knight.hpp"

chess::Knight::Knight(PieceColor color)
    : Piece(color)
{
}

char chess::Knight::getFenCharacter() const
{
    return getFenPieceCharacter('n');
}

bool chess::Knight::movePossible(const Board& board, const Move& move) const
{
    return false;
}
