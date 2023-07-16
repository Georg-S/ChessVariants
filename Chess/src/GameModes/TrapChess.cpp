#include "GameModes/TrapChess.hpp"

#include <cassert>

#include "Utility.hpp"
#include "GameLogic.hpp"

using namespace chess;

void chess::TrapChessRenderer::render(const TrapChessRenderingInformation& renderInfo)
{
	if (!m_running)
		return;

	m_sdlHandler->clear();

	if (renderInfo.promotionSelectionColor)
		render_promotion_selection(*renderInfo.promotionSelectionColor);
	else
		renderTrapChess(renderInfo);

	m_sdlHandler->update();
}

void chess::TrapChessRenderer::renderTrapChess(const TrapChessRenderingInformation& renderInfo)
{
	renderChessBoard();

	if (m_renderPreviousMove && renderInfo.previousMove)
		renderPreviousMove(*renderInfo.previousMove);

	if (renderInfo.positionToRenderOnMousePosition && renderInfo.mousePos)
	{
		renderPiecesWithSelectedOnMousePositionWithBombs(renderInfo);
		renderAllPossibleMovesForSelectedPiece(renderInfo.board, *renderInfo.positionToRenderOnMousePosition);
	}
	else
	{
		renderPieces(renderInfo.board);
		renderBombsExceptSelected(renderInfo.bombPositions);
	}

	if (renderInfo.playerWon)
		renderGameOver(*renderInfo.playerWon);
}

void chess::TrapChessRenderer::renderBombsExceptSelected(const std::vector<Position>& bombPositions, std::optional<Position> selected)
{
	for (const auto& bomb : bombPositions)
	{
		if (selected && *selected == bomb)
			continue;
		renderBomb(bomb);
	}
}

void chess::TrapChessRenderer::renderBomb(const Position& pos)
{
	renderBombWithAppliedOffset(PIECE_WIDTH * pos.x, PIECE_HEIGHT * pos.y);
}

void chess::TrapChessRenderer::renderBombWithAppliedOffset(int windowX, int windowY)
{
	static constexpr int BOMB_WIDTH = static_cast<int>(PIECE_WIDTH / 1.3);
	static constexpr int BOMB_HEIGHT = static_cast<int>(PIECE_HEIGHT / 1.3);
	const int offsetX = -PIECE_WIDTH / 8;
	const int offsetY = -PIECE_HEIGHT / 8;

	const std::string fileString = imagesPath + "bomb.png";
	m_sdlHandler->createAndPushBackRenderElement(fileString, windowX + offsetX, windowY + offsetY, BOMB_WIDTH, BOMB_HEIGHT);
}

static bool isBombSelected(const std::vector<Position>& bombs, const Position& pos)
{
	for (const auto& bomb : bombs)
	{
		if (pos == bomb)
			return true;
	}
	return false;
}

void chess::TrapChessRenderer::renderPiecesWithSelectedOnMousePositionWithBombs(const TrapChessRenderingInformation& renderInfo)
{
	const Board& board = renderInfo.board;
	const Position& selectedPiece = *renderInfo.positionToRenderOnMousePosition;
	char foreGroundPiece{};
	Position pos = {};
	for (pos.x = 0; pos.x < BOARD_WIDTH; pos.x++)
	{
		for (pos.y = 0; pos.y < BOARD_HEIGHT; pos.y++)
		{
			auto piece = board[pos];
			if (!piece)
				continue;

			if (selectedPiece == pos)
				foreGroundPiece = piece->getFenCharacter();
			else
				renderPiece(board[pos]->getFenCharacter(), pos);
		}
	}
	renderBombsExceptSelected(renderInfo.bombPositions, selectedPiece);
	render_piece_on_mouse_position(foreGroundPiece, *renderInfo.mousePos);
	if (isBombSelected(renderInfo.bombPositions, selectedPiece))
		renderBombWithAppliedOffset(renderInfo.mousePos->x - PIECE_WIDTH / 2, renderInfo.mousePos->y - PIECE_HEIGHT / 2);
}

chess::TrapChess::TrapChess(PieceColor playerColor)
	: m_playerPerspective(playerColor)
{
	m_isGameReady = false; // Bombs must be placed first before the game can be played
	TrapChess::setGameState(defaultStartPos);
}

void chess::TrapChess::enableRendering()
{
	m_renderer.start("Trap Chess");
}

bool chess::TrapChess::update()
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

bool chess::TrapChess::isMovePossible(const Move& move) const
{
	if (!m_board[move.from])
		return false;

	if (m_board[move.from]->getColor() != m_currentPlayer)
		return false;

	return ::isMovePossible(m_board, move);
}

static void moveBomb(std::array<std::array<bool, BOARD_HEIGHT>, BOARD_WIDTH>& bombs, const Move& move) 
{
	bombs[move.to.x][move.to.y] = bombs[move.from.x][move.from.y];
	bombs[move.from.x][move.from.y] = false;
}

void chess::TrapChess::makeMove(const Move& move)
{
	auto piece = m_board[move.from];
	if (!piece || piece->getColor() != m_currentPlayer)
		return;

	const bool isEnPassantMove = isPawn(piece) && m_board.enPassantPossible(move.to);
	const auto diff = move.to - move.from;
	Position toCheckTrapPosition = move.to;
	if (isEnPassantMove)
	{
		Position moveDirection = { 0, -1 };
		if (diff.y > 0)
			moveDirection.y = 1;
		toCheckTrapPosition = move.to - moveDirection;
	}

	const auto absDiff = abs(diff);
	const bool isCastlingMove = isKing(piece) && absDiff.x == 2;
	if (isCastlingMove) 
	{
		auto towerMove = getCastlingTowerMove(move);
		if (m_currentPlayer == PieceColor::WHITE)
			moveBomb(m_whitePlayerBomb, towerMove);
		if (m_currentPlayer == PieceColor::BLACK)
			moveBomb(m_blackPlayerBomb, towerMove);
	}

	::makeMove(&m_board, move);

	auto handleBombs = [this, &toCheckTrapPosition, &move](auto& currentPlayerBombs, auto& otherPlayerBombs)
	{
		moveBomb(currentPlayerBombs, move);

		if (otherPlayerBombs[toCheckTrapPosition.x][toCheckTrapPosition.y])
		{
			m_board.removePiece(move.to);
			currentPlayerBombs[move.to.x][move.to.y] = false;
			otherPlayerBombs[toCheckTrapPosition.x][toCheckTrapPosition.y] = false;
		}
	};

	if (m_currentPlayer == PieceColor::WHITE)
		handleBombs(m_whitePlayerBomb, m_blackPlayerBomb);
	if (m_currentPlayer == PieceColor::BLACK)
		handleBombs(m_blackPlayerBomb, m_whitePlayerBomb);

	m_renderInfo.previousMove = move;

	if (!isInPromotion())
		m_currentPlayer = getNextPlayer(m_currentPlayer);
}

bool chess::TrapChess::isGameOver() const
{
	if (!getKingPosition(m_board, PieceColor::BLACK) || !getKingPosition(m_board, PieceColor::WHITE))
		return true;

	return ::isGameOver(m_board, m_currentPlayer);
}

void chess::TrapChess::setPreviousMove(const Move& move)
{
	m_renderInfo.previousMove = move;
}

void chess::TrapChess::setGameState(const std::string& fenString)
{
	m_blackPlayerBomb = {};
	m_whitePlayerBomb = {};

	auto setBombFromString = [](auto& bombsArr, const std::string& bombStr)
	{
		auto pos = toPositionFromChessString(bombStr);
		if (!pos)
			return;

		bombsArr[pos->x][pos->y] = true;
	};

	auto splitted = stringSplit(fenString, " ");
	Game::setGameState(fenString);

	for (size_t i = 4; i < splitted.size(); i++)
	{
		if (i < 4 + MaxAllowedBombsPerPlayer)
			setBombFromString(m_whitePlayerBomb, splitted[i]);
		else if (i < 4 + 2 * MaxAllowedBombsPerPlayer)
			setBombFromString(m_blackPlayerBomb, splitted[i]);
	}
}

std::string chess::TrapChess::getGameState() const
{
	auto getBombString = [](const auto& bombsArr)
	{
		std::string result;
		int count = 0;
		auto bombs = getBombPositions(bombsArr);
		for (const auto& bomb : bombs)
		{
			result += " " + toChessPositionString(bomb);
			count++;
		}

		while (count < 3)
		{
			result += " -";
			count++;
		}

		return result;
	};

	auto boardStateStr = m_board.getFenString(m_currentPlayer);
	auto resultStr = boardStateStr + getBombString(m_whitePlayerBomb) + getBombString(m_blackPlayerBomb);

	return resultStr;
}

static int getCount(const std::array<std::array<bool, BOARD_HEIGHT>, BOARD_WIDTH>& arr)
{
	int count = 0;
	for (int x = 0; x < std::size(arr); x++)
	{
		for (int y = 0; y < std::size(arr[x]); y++)
		{
			if (arr[x][y])
				count++;
		}
	}
	return count;
}

void chess::TrapChess::placeBomb(const Position& pos, PieceColor color)
{
	auto piece = m_board[pos];
	if (!piece || piece->getColor() != color)
		return;

	if (color == PieceColor::BLACK)
	{
		if (getCount(m_blackPlayerBomb) < MaxAllowedBombsPerPlayer)
			m_blackPlayerBomb[pos.x][pos.y] = true;
	}
	else if (color == PieceColor::WHITE)
	{
		if (getCount(m_whitePlayerBomb) < MaxAllowedBombsPerPlayer)
			m_whitePlayerBomb[pos.x][pos.y] = true;
	}

	if (getCount(m_whitePlayerBomb) == MaxAllowedBombsPerPlayer && getCount(m_blackPlayerBomb) == MaxAllowedBombsPerPlayer)
		m_isGameReady = true;
}

void chess::TrapChess::updateRenderInfo()
{
	m_renderInfo.board = m_board.getDeepCopy();
	m_renderInfo.positionToRenderOnMousePosition = m_selectedPiece;
	m_renderInfo.mousePos = { m_mouse.getMousePositionX(), m_mouse.getMousePositionY() };
	m_renderInfo.promotionSelectionColor = getPromotionSelectionColor(m_board);
	if (m_playerPerspective == PieceColor::NONE) 
	{
		m_renderInfo.bombPositions = getBombPositions(m_whitePlayerBomb);
		auto blackBombs = getBombPositions(m_blackPlayerBomb);
		m_renderInfo.bombPositions.insert(m_renderInfo.bombPositions.end(), blackBombs.begin(), blackBombs.end());
	}
	else if (m_playerPerspective == PieceColor::WHITE) 
	{
		m_renderInfo.bombPositions = getBombPositions(m_whitePlayerBomb);
	}
	else if (m_playerPerspective == PieceColor::BLACK) 
	{
		m_renderInfo.bombPositions = getBombPositions(m_blackPlayerBomb);
	}

	if (!getKingPosition(m_board, PieceColor::BLACK))
		m_renderInfo.playerWon = PieceColor::WHITE;
	else if (!getKingPosition(m_board, PieceColor::WHITE))
		m_renderInfo.playerWon = PieceColor::BLACK;
	else if (isCheckMate(m_board, PieceColor::BLACK))
		m_renderInfo.playerWon = PieceColor::WHITE;
	else if (isCheckMate(m_board, PieceColor::WHITE))
		m_renderInfo.playerWon = PieceColor::BLACK;
	else if (isStaleMate(m_board, m_currentPlayer))
		m_renderInfo.playerWon = PieceColor::NONE;
}

std::vector<Position> chess::TrapChess::getBombPositions(const std::array<std::array<bool, BOARD_HEIGHT>, BOARD_WIDTH>& bombs)
{
	std::vector<Position> result;

	for (int x = 0; x < BOARD_WIDTH; x++)
	{
		for (int y = 0; y < BOARD_HEIGHT; y++)
		{
			if (bombs[x][y])
				result.emplace_back(Position{ x,y });
		}
	}

	return result;
}
