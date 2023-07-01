#pragma once
#include <optional>

#include "Board.hpp"
#include "Renderer.hpp"
#include "SDL/Mouse.hpp"

namespace chess
{
	enum class GAME_MODES
	{
		NORMAL, SWAP, TRAP, FOGOFWAR
	};

	std::string gameModeToString(GAME_MODES gameMode);

	class Game
	{
	public:
		virtual ~Game() = default;
		virtual void enableRendering() = 0;
		virtual bool update() = 0; // Should return true if the window is still open
		virtual bool isMovePossible(const Move& move) const = 0;
		virtual void makeMove(const Move& move) = 0;
		virtual bool isGameOver() const = 0;
		virtual void setPreviousMove(const Move& move) = 0;
		virtual void setGameState(const std::string& fenString);
		virtual void pawnPromotion(const Position& selectedPromotionPiece);
		virtual bool isGameReady() const;
		virtual std::string getGameState() const;

		void setGameReady(bool ready);
		std::optional<Position> getSelectedBoardPosition() const;
		std::optional<Position> getSelectedPromotionPosition() const;
		std::optional<Position> getSelectedPiecePosition() const;
		PieceColor getCurrentPlayer() const;
		void selectPiece(const Position& pos);
		void deselectPiece();
		bool isPieceSelected() const;
		bool isInPromotion() const;

	protected:
		std::string getFenString() const;
		char getPieceFromPromotion(const Position& pos) const;

		bool m_isGameReady = true;
		PieceColor m_currentPlayer = PieceColor::NONE;
		std::optional<Position> m_selectedPiece;
		Board m_board;
		Mouse m_mouse;
	};
}