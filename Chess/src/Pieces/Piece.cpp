#include "Pieces/Piece.hpp"

#include <ctype.h>
#include <cassert>

Piece::Piece(PieceColor color)
	: m_pieceColor(color)
{
}

char Piece::getFenPieceCharacter(char character) const
{
	if (m_pieceColor == PieceColor::WHITE)
		return toupper(character);
	if (m_pieceColor == PieceColor::BLACK)
		return tolower(character);

	assert(!"Function called on piece with unspecified color");
	return -1;
}
