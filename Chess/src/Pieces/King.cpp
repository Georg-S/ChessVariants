#include "Pieces/King.hpp"

chess::King::King(PieceColor color)
	: Piece(color)
{
}

char chess::King::getFenCharacter() const
{
	return getFenPieceCharacter('k');
}

bool chess::King::movePossible(const Board& board, const Move& move) const
{
	return false;
}
