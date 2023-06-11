#include "Pieces/Rook.hpp"

chess::Rook::Rook(PieceColor color)
    : Piece(color) 
{
}

char chess::Rook::getFenCharacter() const
{
    return getFenPieceCharacter('r');
}
