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

bool chess::Game::update()
{
	m_mouse.update();
	m_renderInfo.board = m_board.getDeepCopy();
	if (m_mouse.isRightPressed())
		deselectPiece();

	m_renderInfo.mousePos = { m_mouse.getMousePositionX(), m_mouse.getMousePositionY() };
	m_renderer.render(m_renderInfo);
	if (m_renderer.isQuit()) 
	{
		m_renderer.close();
		return false;
	}

	return true;
}

std::optional<chess::Position> chess::Game::getSelectedPosition()
{
	if (!m_mouse.isNewLeftClick())
		return std::nullopt;

	auto [mouseX, mouseY] = m_mouse.getMousePosition();

	Position result = {};
	result.x = mouseX / (WINDOW_WIDTH / BOARD_WIDTH);
	result.y = mouseY / (WINDOW_HEIGHT / BOARD_HEIGHT);

	return result;
}

void chess::Game::selectPiece(const Position& pos)
{
	if (m_board.isOccupied(pos))
		m_renderInfo.positionToRenderOnMousePosition = pos;
	else
		m_renderInfo.positionToRenderOnMousePosition = {};
}

void chess::Game::deselectPiece()
{
	m_renderInfo.positionToRenderOnMousePosition = {};
}
