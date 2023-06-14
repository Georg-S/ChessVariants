#include "Pieces/Rook.hpp"

#include "Board.hpp"
#include "GameLogic.hpp"

chess::Rook::Rook(PieceColor color)
    : Piece(color) 
{
}

char chess::Rook::getFenCharacter() const
{
    return getFenPieceCharacter('r');
}

bool chess::Rook::movePossible(const Board& board, const Move& move) const
{
    if ((move.from.x != move.to.x) && (move.from.y != move.to.y))
        return false;

    return chess::directMovePossible(board, move);
}
