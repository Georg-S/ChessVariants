#pragma once
#include "Piece.hpp"

namespace chess
{
	class Pawn : public Piece
	{
	public:
		Pawn(PieceColor color);
		virtual char getFenCharacter() const override;
		virtual bool movePossible(const Board& board, const Move& move) const override;
		virtual std::unique_ptr<Piece> getDeepCopy() const override;
		virtual void makeMove(chess::Board* inOutBoard, const Move& move) const override;

	private:
		bool movePossible(const Board& board, const Move& move, int allowedYDirection, int twoMovesRow) const;
	};
}