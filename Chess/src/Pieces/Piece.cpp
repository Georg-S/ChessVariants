#include "Pieces/Piece.hpp"

#include <ctype.h>
#include <cassert>
#include "Board.hpp"

chess::Piece::Piece(PieceColor color)
	: m_pieceColor(color)
{
}

void chess::Piece::makeMove(chess::Board* inOutBoard, const Move& move) const
{
	inOutBoard->movePiece(move);
	inOutBoard->resetEnPassantPossibility();
	inOutBoard->resetCastlingPossibility(move);
}

chess::PieceColor chess::Piece::getColor() const
{
	return m_pieceColor;
}

char chess::Piece::getFenPieceCharacter(char character) const
{
	if (m_pieceColor == PieceColor::WHITE)
		return toupper(character);
	if (m_pieceColor == PieceColor::BLACK)
		return tolower(character);

	assert(!"Function called on piece with unspecified color");
	return -1;
}
