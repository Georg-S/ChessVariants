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
		void update();

		std::optional<Position> getSelectedPosition();

	private:
		PieceColor m_currentPlayer;
		Board m_board;
		Renderer m_renderer;
		Mouse m_mouse;
	};
}