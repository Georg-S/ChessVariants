#include "Game.hpp"

#include <cassert>
#include "Utility.hpp"

chess::Game::Game(const std::string& fenString)
{
	auto splitted = chess::stringSplit(fenString, " ");
	assert(splitted.size() >= 2);
	m_board = Board(fenString);

	m_currentPlayer = PieceColor::WHITE;
	if (splitted[1] == "b")
		m_currentPlayer = PieceColor::BLACK;
}

void chess::Game::update()
{
	m_renderer.renderBoard(m_board);
	if (m_renderer.isQuit())
		m_renderer.close();
}

std::optional<chess::Position> chess::Game::getSelectedPosition()
{
	m_mouse.update();

	if (!m_mouse.isNewLeftClick())
		return std::nullopt;

	auto [mouseX, mouseY] = m_mouse.getMousePosition();

	Position result = {};
	result.x = mouseX / (WINDOW_WIDTH / BOARD_WIDTH);
	result.y = mouseY / (WINDOW_HEIGHT / BOARD_HEIGHT);

	return result;
}
