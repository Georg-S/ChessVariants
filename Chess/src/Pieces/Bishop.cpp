#include "Pieces/Bishop.hpp"

chess::Bishop::Bishop(PieceColor color)
	: Piece(color)
{
}

char chess::Bishop::getFenCharacter() const
{
	return getFenPieceCharacter('b');
}
