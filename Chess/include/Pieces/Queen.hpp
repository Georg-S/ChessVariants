#pragma once
#include "Piece.hpp"

namespace chess
{
	class Queen : public Piece
	{
	public:
		Queen(PieceColor color);
		virtual char getFenCharacter() const override;
		virtual bool movePossible(const Board& board, const Move& move) const override;
		virtual std::unique_ptr<Piece> getDeepCopy() const override;
	};
}