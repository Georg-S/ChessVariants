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

using namespace chess;

std::string chess::toChessPositionString(const Position& pos)
{
	Position convPos = pos;

	convPos.y = 8 - pos.y; // Convert from internal board coordinates to "official" coordinates
	char column = 'a' + convPos.x;
	std::string rowStr = std::to_string(convPos.y);

	return column + rowStr;
}

std::optional<Position> chess::toPositionFromChessString(const std::string& str)
{
	Position result = { -1, -1 };
	for (char c : str)
	{
		c = tolower(c);
		if (c >= 'a' && c <= 'h')
			result.x = c - 'a';
		if (c >= '1' && c <= '8')
			result.y = '8' - c;	// Convert from "official" coordinates to internal board coordinates
	}

	if (result.x == -1 || result.y == -1)
		return std::nullopt;

	return result;
}

chess::Board::Board(const std::string& fenString)
{
	loadBoardStateFromFenString(fenString);
}

void Board::loadBoardStateFromFenString(const std::string& fenString)
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

void Board::movePiece(const Move& move)
{
	auto& piece = m_board[move.from.x][move.from.y];
	if (!piece) 
	{
		assert(!"Piece not found");
		return;
	}

	if (move.from == move.to)
		return;

	m_board[move.to.x][move.to.y] = std::move(piece);
}

std::string Board::getFenString(PieceColor currentPlayer) const
{
	std::string playerString = "b";
	if (currentPlayer == PieceColor::WHITE)
		playerString = "w";

	return getPiecesFenString() + " " + playerString + " " + getCastlingFenString() + " " + getEnPassantFenString();
}

const Piece* Board::operator[](const Position& pos) const
{
	return m_board[pos.x][pos.y].get();
}

bool Board::hasSameColor(PieceColor color, const Position& pos) const
{
	auto piece = (*this)[pos];
	if (!piece)
		return false;

	return color == piece->getColor();
}

bool Board::enPassantPossible(const Position& position) const
{
	return m_enPassantPossible[position.x][position.y];
}

bool Board::castlingPossible(const Position& position) const
{
	return m_castlingPossible[position.x][position.y];
}

bool Board::isOccupied(const Position& position) const
{
	if (m_board[position.x][position.y])
		return true;

	return false;
}

Board Board::getDeepCopy() const
{
	Board copyBoard = {};
	copyBoard.m_castlingPossible = m_castlingPossible;
	copyBoard.m_enPassantPossible = m_enPassantPossible;

	Position pos = {};
	for (pos.x = 0; pos.x < BOARD_WIDTH; pos.x++) 
	{
		for (pos.y = 0; pos.y < BOARD_HEIGHT; pos.y++) 
		{
			const auto& piece = m_board[pos.x][pos.y];
			if (!piece)
				continue;

			copyBoard.m_board[pos.x][pos.y] = piece->getDeepCopy();
		}
	}

	return copyBoard;
}

void Board::resetCastlingPossibility(const Move& move)
{
	resetCastlingPossibility(move.from);
	resetCastlingPossibility(move.to);
}

void Board::resetCastlingPossibility(const Position& position)
{
	m_castlingPossible[position.x][position.y] = false;
}

void Board::resetEnPassantPossibility()
{
	for (int x = 0; x < BOARD_WIDTH; x++) 
	{
		std::fill(std::begin(m_enPassantPossible[x]), std::end(m_enPassantPossible[x]), false);
	}
}

void Board::setEnPassantPossibility(const Position& position)
{
	m_enPassantPossible[position.x][position.y] = true;
}

void Board::removePiece(const Position& position)
{
	m_board[position.x][position.y] = nullptr;
}

void chess::Board::replacePiece(const Position& position, char fenCharacter)
{
	assert(m_board[position.x][position.y]);
	m_board[position.x][position.y] = createPieceFromFenCharacter(fenCharacter);
}

void chess::Board::swapPieces(const Move& move)
{
	if (!m_board[move.from.x][move.from.y] || !m_board[move.to.x][move.to.y])
		return;

	auto fromPiece = std::move(m_board[move.from.x][move.from.y]);
	auto toPiece = std::move(m_board[move.to.x][move.to.y]);

	m_board[move.from.x][move.from.y] = std::move(toPiece);
	m_board[move.to.x][move.to.y] = std::move(fromPiece);
}

std::string Board::getPiecesFenString() const
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

std::string Board::getCastlingFenString() const
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

std::string Board::getEnPassantFenString() const
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

	return toChessPositionString({xEnPassant, yEnPassant});
}

void Board::setPiecesFromFenString(const std::string& piecesFen)
{
	auto splitted = stringSplit(piecesFen, "/");

	for (int y = 0; y < BOARD_HEIGHT; y++)
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

void Board::setCastlingFromFenString(const std::string& castlingFen)
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

void Board::setEnPassantFromFenString(const std::string& enPassantFen)
{
	auto enPassantPos = toPositionFromChessString(enPassantFen);

	if (enPassantPos)
		m_enPassantPossible[enPassantPos->x][enPassantPos->y] = true;
}

std::unique_ptr<Piece> Board::createPieceFromFenCharacter(char fenCharacter)
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
