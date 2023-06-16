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

std::unique_ptr<chess::Piece> chess::Rook::getDeepCopy() const
{
    return std::make_unique<Rook>(m_pieceColor);
}

void chess::Rook::makeMove(chess::Board* inOutBoard, const Move& move) const
{
    inOutBoard->movePiece(move);
}
