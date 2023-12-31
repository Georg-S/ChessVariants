#pragma once
#include <string>
#include <array>
#include <vector>
#include <memory>
#include <optional>

#include "Vec2D.hpp"
#include "Pieces/Piece.hpp"

namespace chess
{


	static constexpr int BOARD_WIDTH = 8;
	static constexpr int BOARD_HEIGHT = 8;

	class Board
	{
	public:
		Board() = default;
		Board(const std::string& fenString);
		void loadBoardStateFromFenString(const std::string& fenString);
		void movePiece(const Move& move);
		std::string getFenString(PieceColor currentPlayer) const;
		const Piece* operator[](const Position& pos) const;
		bool hasSameColor(PieceColor color, const Position& pos) const;
		bool enPassantPossible(const Position& position) const;
		bool castlingPossible(const Position& position) const;
		bool isOccupied(const Position& position) const;
		Board getDeepCopy() const;
		void resetCastlingPossibility(const Move& move);
		void resetCastlingPossibility(const Position& position);
		void resetEnPassantPossibility();
		void setEnPassantPossibility(const Position& position);
		void removePiece(const Position& position);
		void replacePiece(const Position& position, char fenCharacter);
		void swapPieces(const Move& move);

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