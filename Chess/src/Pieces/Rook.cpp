#include "Pieces/Rook.hpp"

#include "Board.hpp"
#include "GameLogic.hpp"

using namespace chess;

chess::Rook::Rook(PieceColor color)
    : Piece(color) 
{
}

char Rook::getFenCharacter() const
{
    return getFenPieceCharacter('r');
}

bool Rook::movePossible(const Board& board, const Move& move) const
{
    if ((move.from.x != move.to.x) && (move.from.y != move.to.y))
        return false;

    return directMovePossible(board, move);
}

std::unique_ptr<Piece> Rook::getDeepCopy() const
{
    return std::make_unique<Rook>(m_pieceColor);
}
