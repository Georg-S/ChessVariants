#include "Board.hpp"

#include <iostream>
#include <cassert>
#include "Utility.hpp"
#include "Pieces/Bishop.hpp"
#include "Pieces/King.hpp"
#include "Pieces/Knight.hpp"
#include "Pieces/Pawn.hpp"
#include "Pieces/Queen.hpp"
#include "Pieces/Rook.hpp"

chess::Board::Board(const std::string& fenString)
{
	loadBoardStateFromFenString(fenString);
}

void chess::Board::loadBoardStateFromFenString(const std::string& fenString)
{
	auto splitted = stringSplit(fenString, " ");
	assert(!splitted.empty());
	if (splitted.empty())
	{
		std::cout << "Invalid fen string size" << std::endl;
		return;
	}

	if (!splitted.empty())
		setPiecesFromFenString(splitted[0]);
	if (splitted.size() >= 3)
		setCastlingFromFenString(splitted[2]);
	if (splitted.size() >= 4)
		setEnPassantFromFenString(splitted[3]);
}

std::string chess::Board::getFenString(PieceColor currentPlayer)
{
	std::string playerString = "b";
	if (currentPlayer == PieceColor::WHITE)
		playerString = "w";

	return getPiecesFenString() + " " + playerString + " " + getCastlingFenString() + " " + getEnPassantFenString();
}

const chess::Piece* chess::Board::operator[](const Position& pos) const
{
	return m_board[pos.x][pos.y].get();
}


std::string chess::Board::getPiecesFenString() const
{
	std::string result;
	int counter = 0;

	for (int y = 0; y < BOARD_HEIGHT; y++)
	{
		counter = 0;
		for (int x = 0; x < BOARD_WIDTH; x++)
		{
			if (m_board[x][y])
			{
				if (counter)
					result += std::to_string(counter);
				counter = 0;
				result += m_board[x][y]->getFenCharacter();
			}
			else
			{
				++counter;
			}
		}
		if (counter)
			result += std::to_string(counter);

		if (y < (BOARD_HEIGHT - 1))
			result += "/";
	}

	return result;
}

std::string chess::Board::getCastlingFenString() const
{
	std::string castlingString;
	if (m_castlingPossible[7][7])
		castlingString += 'K';
	if (m_castlingPossible[0][7])
		castlingString += 'Q';
	if (m_castlingPossible[7][0])
		castlingString += 'k';
	if (m_castlingPossible[0][0])
		castlingString += 'q';

	if (castlingString.empty())
		return "-";

	return castlingString;
}

std::string chess::Board::getEnPassantFenString() const
{
	int xEnPassant = -1;
	int yEnPassant = -1;
	for (int y = 0; y < BOARD_HEIGHT; y++)
	{
		for (int x = 0; x < BOARD_WIDTH; x++)
		{
			if (m_enPassantPossible[x][y])
			{
				xEnPassant = x;
				yEnPassant = y;
			}
		}
	}

	if (xEnPassant == -1 || yEnPassant == -1)
		return "-";

	yEnPassant = 8 - yEnPassant; // Convert from internal board coordinates to "official" coordinates
	char column = 'a' + xEnPassant;
	std::string rowStr = std::to_string(yEnPassant);

	return column + rowStr;
}

void chess::Board::setPiecesFromFenString(const std::string& piecesFen)
{
	auto splitted = stringSplit(piecesFen, "/");

	for (int y = 0; y < chess::BOARD_HEIGHT; y++)
	{
		int x_index = 0;
		for (char c : splitted[y])
		{
			if (isdigit(c))
			{
				x_index += c - '0';
				continue;
			}

			m_board[x_index][y] = createPieceFromFenCharacter(c);
			x_index++;
		}
	}

}

void chess::Board::setCastlingFromFenString(const std::string& castlingFen)
{
	for (char c : castlingFen)
	{
		// TODO maybe needs to be split up into black and white castling possible? But most likely not
		if (c == 'k')
			m_castlingPossible[7][0] = true;
		if (c == 'q')
			m_castlingPossible[0][0] = true;
		if (c == 'K')
			m_castlingPossible[7][7] = true;
		if (c == 'Q')
			m_castlingPossible[0][7] = true;
	}
}

void chess::Board::setEnPassantFromFenString(const std::string& enPassantFen)
{
	int x = -1;
	int y = -1;
	for (char c : enPassantFen)
	{
		c = tolower(c);
		if (c >= 'a' && c <= 'h')
			x = c - 'a';
		if (c >= '1' && c <= '8')
			y = '8' - c;	// Convert from "official" coordinates to internal board coordinates
	}

	if (x != -1 && y != -1)
		m_enPassantPossible[x][y] = true;
}

std::unique_ptr<chess::Piece> chess::Board::createPieceFromFenCharacter(char fenCharacter)
{
	PieceColor color = PieceColor::BLACK;
	if (isupper(fenCharacter))
		color = PieceColor::WHITE;

	fenCharacter = tolower(fenCharacter);
	switch (fenCharacter)
	{
	case 'p':	return std::make_unique<Pawn>(color);
	case 'r':	return std::make_unique<Rook>(color);
	case 'q':	return std::make_unique<Queen>(color);
	case 'k':	return std::make_unique<King>(color);
	case 'b':	return std::make_unique<Bishop>(color);
	case 'n':	return std::make_unique<Knight>(color);
	default:	break;
	}

	assert(!"Invalid character input");
	return nullptr;
}
