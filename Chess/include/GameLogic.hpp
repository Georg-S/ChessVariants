#pragma once
#include "Board.hpp"

namespace chess 
{
	void makeMove(Board* inOutBoard, const Move& move);
	bool isCheck(const Board& board, PieceColor color);
	bool isCheck(const Board& board, const Position& kingPos);
	bool directMovePossible(const Board& board, const Move& move);
	bool isMovePossible(const Board& board, const Move& move);
	std::vector<Move> getAllMovesPossible(const Board& board, const Position& piecePosition);
}