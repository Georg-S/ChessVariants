#include "Pieces/Knight.hpp"

#include "Board.hpp"

chess::Knight::Knight(PieceColor color)
    : Piece(color)
{
}

char chess::Knight::getFenCharacter() const
{
    return getFenPieceCharacter('n');
}

bool chess::Knight::movePossible(const Board& board, const Move& move) const
{
    auto dif = move.from - move.to;
    const auto absDif = abs(dif);

    if (absDif.x + absDif.y != 3)
        return false;

    if (absDif.x == 0 || absDif.y == 0)
        return false;

    return true;
}

std::unique_ptr<chess::Piece> chess::Knight::getDeepCopy() const
{
    return std::make_unique<Knight>(m_pieceColor);
}

void chess::Knight::makeMove(chess::Board* inOutBoard, const Move& move) const
{
    inOutBoard->movePiece(move);
}
