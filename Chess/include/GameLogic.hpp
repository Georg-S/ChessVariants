#pragma once
#include "Board.hpp"

namespace chess 
{
	bool directMovePossible(const Board& board, const Move& move);
	bool movePossible(const Board& board, const Move& move);
	std::vector<Move> getAllMovesPossible(const Board& board, const Position& piecePosition);
}