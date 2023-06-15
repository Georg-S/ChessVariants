#pragma once
#include "Piece.hpp"

namespace chess
{
	class Pawn : public Piece
	{
	public:
		Pawn(PieceColor color);
		char getFenCharacter() const override;
		bool movePossible(const Board& board, const Move& move) const override;
		virtual std::unique_ptr<Piece> getDeepCopy() const override;
	};
}