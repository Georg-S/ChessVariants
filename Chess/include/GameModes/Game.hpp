#pragma once
#include <optional>

#include "Board.hpp"
#include "Renderer.hpp"
#include "SDL/Mouse.hpp"

namespace chess
{
	enum class GAME_MODES
	{
		NORMAL, SWAP
	};

	class Game
	{
	public:
		virtual ~Game() = default;
		virtual void enableRendering() = 0;
		virtual bool update() = 0; // Returns true if the window is still open
		virtual bool isMovePossible(const Move& move) const = 0;
		virtual void makeMove(const Move& move) = 0;
		virtual void setGameState(const std::string& fenString);

		std::optional<Position> getSelectedBoardPosition() const;
		std::optional<Position> getSelectedPromotionPosition() const;
		std::optional<Position> getSelectedPiecePosition() const;
		PieceColor getCurrentPlayer() const;
		void selectPiece(const Position& pos);
		void selectPieceForPromotion(const Position& selectedPos);
		void deselectPiece();
		bool isPieceSelected() const;
		bool isInPromotion() const;
		std::string getFenString() const;

	protected:
		char getPieceFromPromotion(const Position& pos) const;

		std::optional<Position> m_selectedPiece;
		PieceColor m_currentPlayer;
		Board m_board;
		Mouse m_mouse;
	};
}