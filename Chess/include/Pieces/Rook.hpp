#pragma once
#include "Piece.hpp"

namespace chess
{
	class Rook : public Piece
	{
	public:
		Rook(PieceColor color);
		char getFenCharacter() const override;
	};
}