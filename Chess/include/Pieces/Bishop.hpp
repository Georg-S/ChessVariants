#pragma once
#include "Piece.hpp"

namespace chess 
{
	class Bishop : public Piece 
	{
	public:
		Bishop(PieceColor color);
		char getFenCharacter() const override;
		bool movePossible(const Board& board, const Move& move) const override;
		virtual std::unique_ptr<Piece> getDeepCopy() const override;
	};
}