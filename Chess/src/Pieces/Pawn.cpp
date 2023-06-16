#include "Pieces/Pawn.hpp"

#include "Board.hpp"

using namespace chess;

chess::Pawn::Pawn(PieceColor color) 
    : Piece(color)
{
}

char Pawn::getFenCharacter() const
{
    return getFenPieceCharacter('p');
}

bool Pawn::movePossible(const Board& board, const Move& move) const
{
    if (PieceColor::BLACK == m_pieceColor)
        return movePossible(board, move, 1, 1);

    return movePossible(board, move, -1, 6);
}

std::unique_ptr<Piece> Pawn::getDeepCopy() const
{
    return std::make_unique<Pawn>(m_pieceColor);
}

void Pawn::makeMove(Board* inOutBoard, const Move& move) const
{
    const bool isEnPassantMove = inOutBoard->enPassantPossible(move.to);
    inOutBoard->resetCastlingPossibility(move);
    inOutBoard->resetEnPassantPossibility();

    const auto diff = move.to - move.from;
    const auto absDiff = abs(diff);

    inOutBoard->movePiece(move);

    Position moveDirection = { 0, -1 };
    if (diff.y > 0)
        moveDirection.y = 1;

    if (!isEnPassantMove) 
    {
        if (absDiff.y == 2) 
            inOutBoard->setEnPassantPossibility(move.to - moveDirection);

        return;
    }

    inOutBoard->removePiece(move.to - moveDirection);
}

bool Pawn::movePossible(const Board& board, const Move& move, int allowedYDirection, int twoMovesRow) const
{
    const auto diff = move.to - move.from;
    const auto absDiff = abs(diff);

    if ((allowedYDirection < 0) != (diff.y < 0))
        return false;

    if (absDiff.y == 0 || absDiff.y > 2)
        return false;

    if (absDiff.x > 1)
        return false;

    if (absDiff.x == 1) 
    {
        if (absDiff.y != 1)
            return false;

        auto piece = board[move.to];
        if (piece) 
            return piece->getColor() != m_pieceColor;

        return board.enPassantPossible(move.to);
    }

    const Position direction = {0, allowedYDirection};

    if (board.isOccupied(move.from + direction))
        return false;

    if (absDiff.y == 2) 
    {
        if (move.from.y != twoMovesRow)
            return false;

        if (board.isOccupied(move.from + 2 * direction))
            return false;
    }

    return true;
}
