#include "GameModes/Game.hpp"

#include <cassert>
#include "Utility.hpp"
#include "GameLogic.hpp"

using namespace chess;

std::string chess::gameModeToString(GAME_MODES gameMode)
{
	switch (gameMode)
	{
	case GAME_MODES::NORMAL: return "NormalChess";
	case GAME_MODES::SWAP: return "SwapChess";
	case GAME_MODES::TRAP: return "TrapChess";
	case GAME_MODES::FOGOFWAR: return "FogOfWarChess";
	default: assert(!"Unrecognized mode"); return "InvalidMode";
	}
}

GAME_MODES chess::gameModeFromString(const std::string& gameModeStr)
{
	if (gameModeStr == "NormalChess") return GAME_MODES::NORMAL;
	if (gameModeStr == "SwapChess") return GAME_MODES::SWAP;
	if (gameModeStr == "TrapChess") return GAME_MODES::TRAP;
	if (gameModeStr == "FogOfWarChess") return GAME_MODES::FOGOFWAR;

	assert(!"Unrecognized game mode string");
	return GAME_MODES(-1);
}

void chess::Game::setGameReady(bool ready)
{
	m_isGameReady = ready;
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

std::optional<Position> chess::Game::getSelectedPiecePosition() const
{
	return m_selectedPiece;
}

PieceColor chess::Game::getCurrentPlayer() const
{
	return m_currentPlayer;
}

void chess::Game::setGameState(const std::string& fenString)
{
	auto splitted = stringSplit(fenString, " ");
	assert(splitted.size() >= 2);
	m_board = Board(fenString);

	m_currentPlayer = PieceColor::WHITE;
	if (splitted[1] == "b")
		m_currentPlayer = PieceColor::BLACK;
}

void chess::Game::pawnPromotion(const Position& selectedPromotionPiece)
{
	auto piece = getPieceFromPromotion(selectedPromotionPiece);
	executePromotion(&m_board, piece);
	m_currentPlayer = getNextPlayer(m_currentPlayer);
}

bool chess::Game::isGameReady() const
{
	return m_isGameReady;
}

std::string chess::Game::getGameState() const
{
	return getFenString();
}

void Game::selectPiece(const Position& pos)
{
	if (m_board.isOccupied(pos) && (m_board[pos]->getColor() == m_currentPlayer))
		m_selectedPiece = pos;
	else
		m_selectedPiece = std::nullopt;
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

std::string chess::Game::getFenString() const
{
	return m_board.getFenString(m_currentPlayer);
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
