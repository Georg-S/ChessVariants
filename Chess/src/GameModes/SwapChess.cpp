#include "GameModes/SwapChess.hpp"

#include "GameLogic.hpp"

using namespace chess;

static bool isSwapMove(const Board& board, const Move& move) 
{
	auto fromPiece = board[move.from];
	auto toPiece = board[move.to];
	if (!fromPiece || !toPiece)
		return false;

	return fromPiece->getColor() == toPiece->getColor();
}

static bool isSwapMovePossible(const Board& board, const Move& move) 
{
	if (move.from == move.to)
		return false;

	if (!board[move.from])
		return false;

	const auto currentPlayer = board[move.from]->getColor();

	if (!isSwapMove(board, move))
		return false;

	auto copyBoard = board.getDeepCopy();
	copyBoard.swapPieces(move);

	return !isCheck(copyBoard, currentPlayer);
}

static bool isSwapChessMovePossible(const Board& board, const Move& move) 
{
	if (!board[move.from])
		return false;

	bool possible = ::isMovePossible(board, move);
	if (possible)
		return true; // Normal move is possible

	return isSwapMovePossible(board, move);
}

static std::vector<Move> getAllPossibleSwapChessMoves(const Board& board, PieceColor currentPlayer) 
{
	std::vector<Move> result;

	Move move = {};
	for (move.from.x = 0; move.from.x < BOARD_WIDTH; move.from.x++)
	{
		for (move.from.y = 0; move.from.y < BOARD_HEIGHT; move.from.y++)
		{
			for (move.to.x = 0; move.to.x < BOARD_WIDTH; move.to.x++)
			{
				for (move.to.y = 0; move.to.y < BOARD_HEIGHT; move.to.y++)
				{
					if (!board[move.from] || board[move.from]->getColor() != currentPlayer)
						continue;

					if (isSwapChessMovePossible(board, move))
						result.emplace_back(move);
				}
			}
		}
	}

	return result;
}

bool chess::SwapChess::update()
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

chess::SwapChess::SwapChess(const std::string& fenString)
{
	setGameState(fenString);
}

void chess::SwapChess::enableRendering()
{
	m_renderer.start("Swap Chess");
}

bool chess::SwapChess::isMovePossible(const Move& move) const
{
	if (m_board[move.from]->getColor() != m_currentPlayer)
		return false;

	return isSwapChessMovePossible(m_board, move);
}

void chess::SwapChess::makeMove(const Move& move)
{
	if (isSwapMove(m_board, move)) 
		m_board.swapPieces(move);
	else 
		::makeMove(&m_board, move);

	m_renderInfo.previousMove = move;

	if (!isInPromotion())
		m_currentPlayer = getNextPlayer(m_currentPlayer);
}

bool chess::SwapChess::isGameOver() const
{
	return isCheckMate(m_board, m_currentPlayer) || isStaleMate(m_board, m_currentPlayer);
}

void chess::SwapChess::setPreviousMove(const Move& move)
{
	m_renderInfo.previousMove = move;
}

bool chess::SwapChess::isCheckMate(const Board& board, PieceColor color) const
{
	if (!isCheck(board, color))
		return false;

	return getAllPossibleSwapChessMoves(board, color).empty();
}

bool chess::SwapChess::isStaleMate(const Board& board, PieceColor color) const
{
	if (isCheck(board, color))
		return false;

	return getAllPossibleSwapChessMoves(board, color).empty();
}

void chess::SwapChess::updateRenderInfo()
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
