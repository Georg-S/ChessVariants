#pragma once
#include <string>
#include <array>
#include <vector>
#include <memory>

#include "Vec2D.hpp"
#include "Pieces/Piece.hpp"

namespace chess
{
	using Position = Vec2D<int>;

	struct Move 
	{
		Position from = {};
		Position to = {};
	};

	static constexpr int BOARD_WIDTH = 8;
	static constexpr int BOARD_HEIGHT = 8;
	static const std::string defaultStartPos = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

	class Board
	{
	public:
		Board() = default;
		Board(const std::string& fenString);
		void loadBoardStateFromFenString(const std::string& fenString);
		std::string getFenString(PieceColor currentPlayer);
		const Piece* operator[](const Position& pos) const;
		bool hasSameColor(PieceColor color, const Position& pos) const;
		bool enPassantPossible(const Position& position) const;
		bool isOccupied(const Position& position) const;
		Board getDeepCopy() const;

	private:
		std::string getPiecesFenString() const;
		std::string getCastlingFenString() const;
		std::string getEnPassantFenString() const;
		void setPiecesFromFenString(const std::string& piecesFen);
		void setCastlingFromFenString(const std::string& castlingFen);
		void setEnPassantFromFenString(const std::string& enPassantFen);
		std::unique_ptr<Piece> createPieceFromFenCharacter(char fenCharacter);

		std::array<std::array<std::unique_ptr<Piece>, BOARD_HEIGHT>, BOARD_WIDTH> m_board;
		std::array<std::array<bool, BOARD_HEIGHT>, BOARD_WIDTH> m_castlingPossible = {};
		std::array<std::array<bool, BOARD_HEIGHT>, BOARD_WIDTH> m_enPassantPossible = {};
	};
}