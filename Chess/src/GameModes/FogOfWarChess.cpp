#include "GameModes/FogOfWarChess.hpp"

using namespace chess;

#include "GameLogic.hpp"

void chess::FogOfWarRenderer::render(const FogOfWarRenderInformation& renderInfo)
{
	if (!m_running)
		return;

	m_sdlHandler->clear();

	if (renderInfo.promotionSelectionColor)
		render_promotion_selection(*renderInfo.promotionSelectionColor);
	else
		renderFogOfWarGameState(renderInfo);

	m_sdlHandler->update();

}

// Get a board with only pieces visible for the player
Board getFogOfWarBoard(const Board& board, PieceColor playerColor) 
{
	bool isVisible[BOARD_WIDTH][BOARD_HEIGHT] = {};

	auto setIsVisibleForPosition = [&isVisible](const Position& pos) 
	{
		Position offset = {};
		for (offset.x = -1; offset.x <= 1; offset.x++)
		{
			for (offset.y = -1; offset.y <= 1; offset.y++)
			{
				Position visiblePosition = offset + pos;
				if (isIndexInsideBoardBoundaries(visiblePosition))
					isVisible[visiblePosition.x][visiblePosition.y] = true;
			}
		}
	};

	Position pos = {};
	for (pos.x = 0; pos.x < BOARD_WIDTH; pos.x++)
	{
		for (pos.y = 0; pos.y < BOARD_HEIGHT; pos.y++)
		{
			auto piece = board[pos];
			if (!piece || (piece->getColor() != playerColor))
				continue;
			setIsVisibleForPosition(pos);
		}
	}

	Board resultBoard = board.getDeepCopy();
	pos = {};
	for (pos.x = 0; pos.x < BOARD_WIDTH; pos.x++)
	{
		for (pos.y = 0; pos.y < BOARD_HEIGHT; pos.y++)
		{
			if (!isVisible[pos.x][pos.y])
				resultBoard.removePiece(pos);
		}
	}

	return resultBoard;
}

void chess::FogOfWarRenderer::renderFogOfWarGameState(const FogOfWarRenderInformation& renderInfo)
{
	auto renderBoard = getFogOfWarBoard(renderInfo.board, renderInfo.toRenderColor);
	renderChessBoard();

	if (renderInfo.positionToRenderOnMousePosition && renderInfo.mousePos)
		renderPiecesWithSelectedOnMousePosition(renderBoard, *renderInfo.mousePos, *renderInfo.positionToRenderOnMousePosition);
	else
		renderPieces(renderBoard);

	if (renderInfo.playerWon)
		renderGameOver(*renderInfo.playerWon);
}

chess::FogOfWarChess::FogOfWarChess(PieceColor playerColor)
	: m_renderColor(playerColor)
{
	setGameState(defaultStartPos);
}

void chess::FogOfWarChess::enableRendering()
{
	m_renderer.start();
}

bool chess::FogOfWarChess::update()
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

bool chess::FogOfWarChess::isMovePossible(const Move& move) const
{
	if (!m_board[move.from])
		return false;

	if (m_board[move.from]->getColor() != m_currentPlayer)
		return false;

	return ::isMovePossible(m_board, move);
}

void chess::FogOfWarChess::makeMove(const Move& move)
{
	::makeMove(&m_board, move);

	if (!isInPromotion())
		m_currentPlayer = getNextPlayer(m_currentPlayer);
}

bool chess::FogOfWarChess::isGameOver() const
{
	return ::isGameOver(m_board, m_currentPlayer);
}

void chess::FogOfWarChess::setPreviousMove(const Move& move)
{
	// Intentionally do nothing
}

void chess::FogOfWarChess::updateRenderInfo()
{
	m_renderInfo.board = m_board.getDeepCopy();
	m_renderInfo.positionToRenderOnMousePosition = m_selectedPiece;
	m_renderInfo.mousePos = { m_mouse.getMousePositionX(), m_mouse.getMousePositionY() };
	m_renderInfo.promotionSelectionColor = getPromotionSelectionColor(m_board);
	m_renderInfo.toRenderColor = m_renderColor;
	if (isCheckMate(m_board, PieceColor::BLACK))
		m_renderInfo.playerWon = PieceColor::WHITE;
	else if (isCheckMate(m_board, PieceColor::WHITE))
		m_renderInfo.playerWon = PieceColor::BLACK;
	else if (isStaleMate(m_board, m_currentPlayer))
		m_renderInfo.playerWon = PieceColor::NONE;
}
