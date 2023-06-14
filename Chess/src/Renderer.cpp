#include "Renderer.hpp"

#include <cassert>

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
	m_sdlHandler->createAndPushBackRenderElement("Images/Board.png", 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
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

std::string chess::Renderer::getPieceFileString(char fenChar) const
{
	chess::PieceColor color = islower(fenChar) ? chess::PieceColor::BLACK : chess::PieceColor::WHITE;

	return "Images/" + getPieceTypeString(tolower(fenChar)) + "_" + getColorString(color) + ".png";
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
