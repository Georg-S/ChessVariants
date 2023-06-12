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
	for (int x = 0; x < BOARD_WIDTH; x++)
	{
		for (int y = 0; y < BOARD_HEIGHT; y++)
		{
			if (!board[x][y])
				continue;
			
			renderPiece(board[x][y]->getFenCharacter(), x, y);
		}
	}
}

void chess::Renderer::renderPiece(char fenChar, int x, int y)
{
	std::string fileString = getPieceFileString(fenChar);
	m_sdlHandler->createAndPushBackRenderElement(fileString, PIECE_WIDTH * x, PIECE_HEIGHT * y, PIECE_WIDTH, PIECE_HEIGHT);
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
