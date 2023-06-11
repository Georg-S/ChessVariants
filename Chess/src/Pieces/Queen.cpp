#include "Pieces/Queen.hpp"

chess::Queen::Queen(PieceColor color)
    : Piece(color) 
{
}

char chess::Queen::getFenCharacter() const
{
    return getFenPieceCharacter('q');
}
