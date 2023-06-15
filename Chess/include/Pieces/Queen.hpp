#pragma once
#include "Piece.hpp"

namespace chess
{
	class Queen : public Piece
	{
	public:
		Queen(PieceColor color);
		char getFenCharacter() const override;
		bool movePossible(const Board& board, const Move& move) const override;
		virtual std::unique_ptr<Piece> getDeepCopy() const override;
	};
}