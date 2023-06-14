#include "Pieces/Bishop.hpp"

chess::Bishop::Bishop(PieceColor color)
	: Piece(color)
{
}

char chess::Bishop::getFenCharacter() const
{
	return getFenPieceCharacter('b');
}

bool chess::Bishop::movePossible(const Board& board, const Move& move) const
{
	return false;
}
