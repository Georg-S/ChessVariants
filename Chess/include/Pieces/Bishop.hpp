#pragma once
#include "Piece.hpp"

namespace chess 
{
	class Bishop : public Piece 
	{
	public:
		Bishop(PieceColor color);

		char getFenCharacter() const override;
	};
}