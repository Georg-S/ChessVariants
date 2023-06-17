#include "Game.hpp"

#include <cassert>
#include "Utility.hpp"
#include "GameLogic.hpp"

using namespace chess;

chess::Game::Game(const std::string& fenString)
{
	auto splitted = stringSplit(fenString, " ");
	assert(splitted.size() >= 2);
	m_board = Board(fenString);

	m_currentPlayer = PieceColor::WHITE;
	if (splitted[1] == "b")
		m_currentPlayer = PieceColor::BLACK;
}

bool Game::update()
{
	m_mouse.update();
	updateRenderInfo();

	if (m_mouse.isRightPressed())
		deselectPiece();

	m_renderer.render(m_renderInfo);
	if (m_renderer.isQuit()) 
	{
		m_renderer.close();
		return false;
	}

	return true;
}

std::optional<Position> Game::getSelectedBoardPosition() const
{
	if (!m_mouse.isNewLeftClick())
		return std::nullopt;

	auto [mouseX, mouseY] = m_mouse.getMousePosition();

	Position result = {};
	result.x = mouseX / (WINDOW_WIDTH / BOARD_WIDTH);
	result.y = mouseY / (WINDOW_HEIGHT / BOARD_HEIGHT);

	return result;
}

std::optional<Position> chess::Game::getSelectedPromotionPosition() const
{
	if (!m_mouse.isNewLeftClick())
		return std::nullopt;

	Position result = {};
	result.x = m_mouse.getMousePositionX() / (WINDOW_WIDTH / 2);
	result.y = m_mouse.getMousePositionY() / (WINDOW_HEIGHT / 2);

	return result;
}

void Game::selectPiece(const Position& pos)
{
	if (m_board.isOccupied(pos) && (m_board[pos]->getColor() == m_currentPlayer))
		m_selectedPiece = pos;
	else 
		m_selectedPiece = std::nullopt;
}

void Game::selectPieceForPromotion(const Position& selectedPos) 
{
	auto piece = getPieceFromPromotion(selectedPos);
	executePromotion(&m_board, piece);
}

void chess::Game::makeMoveWithSelectedPiece(const Position& to)
{
	if (!m_selectedPiece)
		return;

	const Position fromPosition = *m_selectedPiece;
	deselectPiece();

	if (m_board[fromPosition]->getColor() != m_currentPlayer)
		return;

	const Move move = { fromPosition, to };
	if (!isMovePossible(m_board, move))
		return;

	::makeMove(&m_board, move);
	m_renderInfo.previousMove = move;
	m_currentPlayer = getNextPlayer(m_currentPlayer);
}

void Game::deselectPiece()
{
	m_selectedPiece = std::nullopt;
}

bool Game::isPieceSelected() const
{
	if (m_selectedPiece)
		return true;

	return false;
}

bool chess::Game::isInPromotion() const
{
	if (getPromotionSelectionColor(m_board))
		return true;

	return false;
}

char chess::Game::getPieceFromPromotion(const Position& pos) const
{
	auto promotionSelectionColor = getPromotionSelectionColor(m_board);
	assert(promotionSelectionColor);

	char piece = ' ';
	if (pos.x == 0 && pos.y == 0)
		piece = 'q';
	else if (pos.x == 1 && pos.y == 0)
		piece = 'r';
	else if (pos.x == 0 && pos.y == 1)
		piece = 'n';
	else
		piece = 'b';

	if (*promotionSelectionColor == PieceColor::WHITE)
		piece = toupper(piece);

	return piece;
}

void chess::Game::updateRenderInfo()
{
	m_renderInfo.board = m_board.getDeepCopy();
	m_renderInfo.positionToRenderOnMousePosition = m_selectedPiece;
	m_renderInfo.mousePos = { m_mouse.getMousePositionX(), m_mouse.getMousePositionY() };
	m_renderInfo.promotionSelectionColor = getPromotionSelectionColor(m_board);
}
