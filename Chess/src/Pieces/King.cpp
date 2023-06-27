#include "Pieces/King.hpp"

#include "Board.hpp"
#include "GameLogic.hpp"

using namespace chess;

chess::King::King(PieceColor color)
	: Piece(color)
{
}

char King::getFenCharacter() const
{
	return getFenPieceCharacter('k');
}

static void resetCastlingPossibilityForColor(Board* board, PieceColor color)
{
	Position towerPos1 = { 7, 7 };
	Position towerPos2 = { 0, 7 };
	if (color == PieceColor::BLACK)
	{
		towerPos1 = { 0, 0 };
		towerPos2 = { 7, 0 };
	}

	board->resetCastlingPossibility(towerPos1);
	board->resetCastlingPossibility(towerPos2);
}

bool King::movePossible(const Board& board, const Move& move) const
{
	const auto diff = move.to - move.from;
	const auto absDiff = abs(diff);
	Position direction = { -1, 0 };
	if (diff.x > 0)
		direction.x = 1;

	if (absDiff.x <= 1 && absDiff.y <= 1)
		return true;

	if ((absDiff.x != 2) || (absDiff.y != 0))
		return false;

	const auto castlingTowerMove = getCastlingTowerMove(move);
	if (!board.castlingPossible(castlingTowerMove.from))
		return false;

	if (board.isOccupied(move.from + direction) || board.isOccupied(move.from + 2 * direction))
		return false;

	// We don't check for check in the resulting position, this is done in the GameLogic
	for (Position toCheck = move.from; toCheck != move.to; toCheck += direction)
	{
		Board copyBoard = board.getDeepCopy();
		const Move moveKing = { move.from, toCheck };

		copyBoard.movePiece(moveKing);
		if (isCheck(copyBoard, toCheck))
			return false;
	}

	return true;
}

std::unique_ptr<Piece> King::getDeepCopy() const
{
	return std::make_unique<King>(m_pieceColor);
}

void King::makeMove(Board* inOutBoard, const Move& move) const
{
	inOutBoard->resetEnPassantPossibility();
	resetCastlingPossibilityForColor(inOutBoard, m_pieceColor);

	auto diff = move.to - move.from;
	auto absDiff = abs(diff);
	inOutBoard->movePiece(move);

	if (absDiff.x < 2)
		return;
	
	// Perform tower move for castling
	const auto towerMove = getCastlingTowerMove(move); 
	inOutBoard->movePiece(towerMove);
}
