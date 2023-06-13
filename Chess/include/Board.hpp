#pragma once
#include <string>
#include <array>
#include <vector>
#include <memory>

#include "Pieces/Piece.hpp"

namespace chess
{
	struct Position 
	{
		int x = -1;
		int y = -1;
	};

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
		std::array<std::unique_ptr<Piece>, BOARD_HEIGHT>& operator[](int x);
		const std::array<std::unique_ptr<Piece>, BOARD_HEIGHT>& operator[](int x) const;

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