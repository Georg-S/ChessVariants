#pragma once
#include <optional>

#include "Board.hpp"
#include "Renderer.hpp"
#include "SDL/Mouse.hpp"

namespace chess
{
	class Game
	{
	public:
		Game(const std::string& fenString = defaultStartPos);
		bool update(); // Returns true if the window is still open

		std::optional<Position> getSelectedBoardPosition() const;
		std::optional<Position> getSelectedPromotionPosition() const;
		void selectPiece(const Position& pos);
		void selectPieceForPromotion(const Position& selectedPos);
		void makeMoveWithSelectedPiece(const Position& to);
		void deselectPiece();
		bool isPieceSelected() const;
		bool isInPromotion() const;

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