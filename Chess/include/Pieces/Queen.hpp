#pragma once
#include "Piece.hpp"

namespace chess
{
	class Queen : public Piece
	{
	public:
		Queen(PieceColor color);
		char getFenCharacter() const override;
	};
}