#include "GameModes/Chess.hpp"

#include "GameLogic.hpp"

using namespace chess;

chess::Chess::Chess(const std::string& fenString)
{
	setGameState(fenString);
}

void chess::Chess::enableRendering()
{
	m_renderer.start("Chess");
}

bool chess::Chess::update()
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

bool chess::Chess::isMovePossible(const Move& move) const
{
	if (!m_board[move.from])
		return false;

	if (m_board[move.from]->getColor() != m_currentPlayer)
		return false;

	return ::isMovePossible(m_board, move);
}

void chess::Chess::makeMove(const Move& move)
{
	::makeMove(&m_board, move);
	m_renderInfo.previousMove = move;
	
	if (!isInPromotion())
		m_currentPlayer = getNextPlayer(m_currentPlayer);
}

bool chess::Chess::isGameOver() const
{
	return ::isGameOver(m_board, m_currentPlayer);
}

void chess::Chess::setPreviousMove(const Move& move)
{
	m_renderInfo.previousMove = move;
}

void chess::Chess::updateRenderInfo()
{
	m_renderInfo.board = m_board.getDeepCopy();
	m_renderInfo.positionToRenderOnMousePosition = m_selectedPiece;
	m_renderInfo.mousePos = { m_mouse.getMousePositionX(), m_mouse.getMousePositionY() };
	m_renderInfo.promotionSelectionColor = getPromotionSelectionColor(m_board);
	m_renderInfo.playerWon = {};
	if (isCheckMate(m_board, PieceColor::BLACK))
		m_renderInfo.playerWon = PieceColor::WHITE;
	else if (isCheckMate(m_board, PieceColor::WHITE))
		m_renderInfo.playerWon = PieceColor::BLACK;
	else if (isStaleMate(m_board, m_currentPlayer))
		m_renderInfo.playerWon = PieceColor::NONE;
}
