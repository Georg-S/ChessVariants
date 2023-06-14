#include "Pieces/Bishop.hpp"

#include "GameLogic.hpp"

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
	auto direction = move.to - move.from;
	if (abs(direction.x) != abs(direction.y))
		return false;

	return chess::directMovePossible(board, move);
}
