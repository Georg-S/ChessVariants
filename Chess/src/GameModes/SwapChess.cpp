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

static bool isSwapMovePossible(const Board& board, PieceColor currentPlayer, const Move& move) 
{
	if (!isSwapMove(board, move))
		return false;

	auto copyBoard = board.getDeepCopy();
	copyBoard.swapPieces(move);

	return !isCheck(copyBoard, currentPlayer);
}

void chess::SwapChess::enableRendering()
{
	m_renderer.start("Swap Chess");
}

bool chess::SwapChess::isMovePossible(const Move& move) const
{
	if (!m_board[move.from])
		return false;

	if (m_board[move.from]->getColor() != m_currentPlayer)
		return false;

	bool possible = ::isMovePossible(m_board, move);
	if (possible)
		return true; // Normal move is possible

	return isSwapMovePossible(m_board, m_currentPlayer, move);
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
