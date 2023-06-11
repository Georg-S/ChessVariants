#include "Pieces/Pawn.hpp"

chess::Pawn::Pawn(PieceColor color) 
    : Piece(color)
{
}

char chess::Pawn::getFenCharacter() const
{
    return getFenPieceCharacter('p');
}
