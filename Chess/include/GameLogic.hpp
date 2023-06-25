#pragma once
#include <optional>
#include "Board.hpp"

namespace chess 
{
	bool isIndexInsideBoardBoundaries(const Position& position);
	PieceColor getNextPlayer(PieceColor player);
	void makeMove(Board* inOutBoard, const Move& move);
	bool isCheck(const Board& board, PieceColor color);
	bool isCheck(const Board& board, const Position& kingPos);
	bool directMovePossible(const Board& board, const Move& move);
	bool isMovePossible(const Board& board, const Move& move);
	bool isGameOver(const Board& board, PieceColor color);
	bool isCheckMate(const Board& board, PieceColor color);
	bool isStaleMate(const Board& board, PieceColor color);
	std::vector<Move> getAllPossibleMoves(const Board& board, PieceColor color);
	std::vector<Move> getAllPossibleMoves(const Board& board, const Position& piecePosition);
	std::optional<PieceColor> getPromotionSelectionColor(const Board& board);
	std::optional<Position> getKingPosition(const Board& board, PieceColor color);
	void executePromotion(Board* inOutBoard, char selectedFenCharPiece);
}