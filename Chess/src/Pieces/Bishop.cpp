#include "Pieces/Bishop.hpp"

#include "GameLogic.hpp"

using namespace chess;

chess::Bishop::Bishop(PieceColor color)
	: Piece(color)
{
}

char Bishop::getFenCharacter() const
{
	return getFenPieceCharacter('b');
}

bool Bishop::movePossible(const Board& board, const Move& move) const
{
	auto direction = move.to - move.from;
	if (abs(direction.x) != abs(direction.y))
		return false;

	return directMovePossible(board, move);
}

std::unique_ptr<Piece> Bishop::getDeepCopy() const
{
	return std::make_unique<Bishop>(m_pieceColor);
}
