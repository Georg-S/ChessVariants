#pragma once
#include "Piece.hpp"

namespace chess 
{
	class Knight : public Piece
	{
	public:
		Knight(PieceColor color);
		char getFenCharacter() const override;

	};
}