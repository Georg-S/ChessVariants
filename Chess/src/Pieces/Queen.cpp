#include "Pieces/Queen.hpp"

#include "GameLogic.hpp"

using namespace chess;

chess::Queen::Queen(PieceColor color)
    : Piece(color) 
{
}

char Queen::getFenCharacter() const
{
    return getFenPieceCharacter('q');
}

bool Queen::movePossible(const Board& board, const Move& move) const
{
    return directMovePossible(board, move);
}

std::unique_ptr<Piece> Queen::getDeepCopy() const
{
    return std::make_unique<Queen>(m_pieceColor);
}
