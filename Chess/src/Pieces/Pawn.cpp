#include "Pieces/Pawn.hpp"

#include "Board.hpp"

chess::Pawn::Pawn(PieceColor color) 
    : Piece(color)
{
}

char chess::Pawn::getFenCharacter() const
{
    return getFenPieceCharacter('p');
}

bool chess::Pawn::movePossible(const Board& board, const Move& move) const
{
    if (PieceColor::BLACK == m_pieceColor)
        return movePossible(board, move, 1, 1);

    return movePossible(board, move, -1, 6);
}

std::unique_ptr<chess::Piece> chess::Pawn::getDeepCopy() const
{
    return std::make_unique<Pawn>(m_pieceColor);
}

bool chess::Pawn::movePossible(const Board& board, const Move& move, int allowedYDirection, int twoMovesRow) const
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
