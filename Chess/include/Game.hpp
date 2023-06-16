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

		std::optional<Position> getSelectedPosition();
		void selectPiece(const Position& pos);
		void deselectPiece();

	private:
		RenderInformation m_renderInfo;
		PieceColor m_currentPlayer;
		Board m_board;
		Renderer m_renderer;
		Mouse m_mouse;
	};
}