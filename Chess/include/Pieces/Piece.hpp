#pragma once

namespace chess
{
	enum class PieceColor { WHITE, BLACK, NONE };

	class Piece
	{
	public:
		Piece(PieceColor color);
		virtual ~Piece() = default;
		virtual char getFenCharacter() const = 0;
		PieceColor getColor() const;

	protected:
		char getFenPieceCharacter(char character) const;

		PieceColor m_pieceColor = PieceColor::NONE;
	};
}