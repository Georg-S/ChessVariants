#include "Pieces/Knight.hpp"

chess::Knight::Knight(PieceColor color)
    : Piece(color)
{
}

char chess::Knight::getFenCharacter() const
{
    return getFenPieceCharacter('n');
}
