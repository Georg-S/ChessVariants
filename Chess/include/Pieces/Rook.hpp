#pragma once
#include "Piece.hpp"

namespace chess
{
	class Rook : public Piece
	{
	public:
		Rook(PieceColor color);
		virtual char getFenCharacter() const override;
		virtual bool movePossible(const Board& board, const Move& move) const override;
		virtual std::unique_ptr<Piece> getDeepCopy() const override;
	};
}