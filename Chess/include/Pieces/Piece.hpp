#pragma once

#include <memory>

namespace chess
{
	class Board;
	struct Move;

	enum class PieceColor { WHITE, BLACK, NONE };

	class Piece
	{
	public:
		Piece(PieceColor color);
		virtual ~Piece() = default;
		virtual char getFenCharacter() const = 0;
		virtual bool movePossible(const Board& board, const Move& move) const = 0;
		virtual std::unique_ptr<Piece> getDeepCopy() const = 0;
		virtual void makeMove(chess::Board* inOutBoard, const Move& move) const;
		PieceColor getColor() const;

	protected:
		char getFenPieceCharacter(char character) const;

		PieceColor m_pieceColor = PieceColor::NONE;
	};
}