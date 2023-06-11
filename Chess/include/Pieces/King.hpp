#pragma once
#include "Piece.hpp"

namespace chess
{
	class King : public Piece
	{
	public:
		King(PieceColor color);
		char getFenCharacter() const override;
	};
}