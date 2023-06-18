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
		Game(const std::string& fenString = defaultStartPos);
		void enableRendering();
		bool update(); // Returns true if the window is still open

		std::optional<Position> getSelectedBoardPosition() const;
		std::optional<Position> getSelectedPromotionPosition() const;
		std::optional<Position> getSelectedPiecePosition() const;
		PieceColor getCurrentPlayer() const;
		void setGameState(const std::string& fenString);
		void selectPiece(const Position& pos);
		void selectPieceForPromotion(const Position& selectedPos);
		void makeMoveWithSelectedPiece(const Position& to);
		void makeMove(const Move& move);
		bool isMovePossible(const Move& move) const;
		void deselectPiece();
		bool isPieceSelected() const;
		bool isInPromotion() const;
		std::string getFenString() const;

	private:
		char getPieceFromPromotion(const Position& pos) const;
		void updateRenderInfo();

		std::optional<Position> m_selectedPiece;
		RenderInformation m_renderInfo;
		PieceColor m_currentPlayer;
		Board m_board;
		Renderer m_renderer;
		Mouse m_mouse;
	};
}