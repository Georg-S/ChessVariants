#pragma once
#include "Piece.hpp"

namespace chess
{
	class Pawn : public Piece
	{
	public:
		Pawn(PieceColor color);
		char getFenCharacter() const override;
	};
}