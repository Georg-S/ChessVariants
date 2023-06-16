#include "Pieces/Piece.hpp"

#include <ctype.h>
#include <cassert>
#include "Board.hpp"

using namespace chess;

chess::Piece::Piece(PieceColor color)
	: m_pieceColor(color)
{
}

void Piece::makeMove(Board* inOutBoard, const Move& move) const
{
	inOutBoard->movePiece(move);
	inOutBoard->resetEnPassantPossibility();
	inOutBoard->resetCastlingPossibility(move);
}

PieceColor Piece::getColor() const
{
	return m_pieceColor;
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
