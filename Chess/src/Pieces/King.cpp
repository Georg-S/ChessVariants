#include "Pieces/King.hpp"

#include "Board.hpp"

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
	auto diff = move.from - move.to;
	auto absDiff = abs(diff);
	if (absDiff.x <= 1 && absDiff.y <= 1)
		return true;

	if ((absDiff.x != 2) && (absDiff.y != 0))
		return false; 

	// TODO finish implementation for castling moves, once isCheck is implemented
	return false;
}

std::unique_ptr<chess::Piece> chess::King::getDeepCopy() const
{
	return std::make_unique<King>(m_pieceColor);
}
