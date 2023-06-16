#include "Renderer.hpp"

#include <cassert>
#include "GameLogic.hpp"

static const std::string basePath = "Images/";

chess::Renderer::Renderer()
{
	m_sdlHandler = std::make_unique<SDLHandler>(WINDOW_WIDTH, WINDOW_HEIGHT, true);
	m_sdlHandler->start("Chess");
}

void chess::Renderer::renderBoard(const std::string& fenBoardString)
{
	Board board = Board(fenBoardString);
	renderBoard(board);
}

void chess::Renderer::renderBoard(const chess::Board& board)
{
	m_sdlHandler->clear();
	renderChessBoard();
	renderPieces(board);
	m_sdlHandler->update();
}

void chess::Renderer::render(const RenderInformation& renderInfo)
{
	m_sdlHandler->clear();
	renderChessBoard();

	if (m_renderPreviousMove && renderInfo.previousMove)
		renderPreviousMove(*renderInfo.previousMove);

	if (renderInfo.positionToRenderOnMousePosition && renderInfo.mousePos)
	{
		renderPiecesWithSelectedOnMousePosition(renderInfo.board, *renderInfo.mousePos, *renderInfo.positionToRenderOnMousePosition);
		renderAllPossibleMovesForSelectedPiece(renderInfo.board, *renderInfo.positionToRenderOnMousePosition);
	}
	else 
	{
		renderPieces(renderInfo.board);
	}

	m_sdlHandler->update();
}

bool chess::Renderer::isQuit() const
{
	return m_sdlHandler->isExit();
}

void chess::Renderer::close()
{
	m_sdlHandler->close();
}

void chess::Renderer::renderChessBoard()
{
	m_sdlHandler->createAndPushBackRenderElement(basePath + "Board.png", 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
}

void chess::Renderer::renderPieces(const chess::Board& board)
{
	Position pos = {};
	for (pos.x = 0; pos.x < BOARD_WIDTH; pos.x++)
	{
		for (pos.y = 0; pos.y < BOARD_HEIGHT; pos.y++)
		{
			if (!board[pos])
				continue;
			
			renderPiece(board[pos]->getFenCharacter(), pos);
		}
	}
}

void chess::Renderer::renderPiece(char fenChar, const Position& pos)
{
	std::string fileString = getPieceFileString(fenChar);
	m_sdlHandler->createAndPushBackRenderElement(fileString, PIECE_WIDTH * pos.x, PIECE_HEIGHT * pos.y, PIECE_WIDTH, PIECE_HEIGHT);
}

void chess::Renderer::renderPreviousMove(const chess::Move& previousMove)
{
	m_sdlHandler->createAndPushBackRenderElement(basePath + "PreviousMove.png", PIECE_WIDTH * previousMove.from.x,
		PIECE_HEIGHT * previousMove.from.y, PIECE_WIDTH, PIECE_HEIGHT);

	m_sdlHandler->createAndPushBackRenderElement(basePath + "PreviousMove.png", PIECE_WIDTH * previousMove.to.x,
		PIECE_HEIGHT * previousMove.to.y, PIECE_WIDTH, PIECE_HEIGHT);
}

void chess::Renderer::renderPiecesWithSelectedOnMousePosition(const Board& board, const Position& mousePos, const Position& selectedPiece)
{
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
	render_piece_on_mouse_position(foreGroundPiece, mousePos);
}

void chess::Renderer::render_piece_on_mouse_position(char piece, const Position& mousePos)
{
	std::string fileString = getPieceFileString(piece);
	m_sdlHandler->createAndPushBackRenderElement(fileString, mousePos.x - (PIECE_WIDTH/ 2), mousePos.y - (PIECE_HEIGHT / 2), PIECE_WIDTH, PIECE_HEIGHT);
}

void chess::Renderer::renderAllPossibleMovesForSelectedPiece(const chess::Board& board, const chess::Position& selectedPos)
{
	auto possibleMoves = chess::getAllMovesPossible(board, selectedPos);
	for (const auto& move : possibleMoves) 
	{
		m_sdlHandler->createAndPushBackRenderElement(basePath + "PossibleMove.png", PIECE_WIDTH * move.to.x, 
			PIECE_HEIGHT * move.to.y, PIECE_WIDTH, PIECE_HEIGHT);
	}
}

std::string chess::Renderer::getPieceFileString(char fenChar) const
{
	chess::PieceColor color = islower(fenChar) ? chess::PieceColor::BLACK : chess::PieceColor::WHITE;

	return basePath + getPieceTypeString(tolower(fenChar)) + "_" + getColorString(color) + ".png";
}

std::string chess::Renderer::getPieceTypeString(char fenChar) const
{
	switch (fenChar)
	{
	case 'p':		return "Pawn";
	case 'n':		return "Knight";
	case 'q':		return "Queen";
	case 'k':		return "King";
	case 'b':		return "Bishop";
	case 'r':		return "Rook";
	default:		assert(!"Invalid input");
	}
	return "";
}

std::string chess::Renderer::getColorString(chess::PieceColor color) const
{
	if (color == chess::PieceColor::WHITE)
		return "white";
	else
		return "black";
}