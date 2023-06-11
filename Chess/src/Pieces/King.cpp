#include "Pieces/King.hpp"

chess::King::King(PieceColor color)
	: Piece(color)
{
}

char chess::King::getFenCharacter() const
{
	return getFenPieceCharacter('k');
}
