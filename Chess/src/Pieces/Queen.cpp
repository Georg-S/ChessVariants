#include "Pieces/Queen.hpp"

#include "GameLogic.hpp"

chess::Queen::Queen(PieceColor color)
    : Piece(color) 
{
}

char chess::Queen::getFenCharacter() const
{
    return getFenPieceCharacter('q');
}

bool chess::Queen::movePossible(const Board& board, const Move& move) const
{
    return chess::directMovePossible(board, move);
}

std::unique_ptr<chess::Piece> chess::Queen::getDeepCopy() const
{
    return std::make_unique<Queen>(m_pieceColor);
}

void chess::Queen::makeMove(chess::Board* inOutBoard, const Move& move) const
{
    inOutBoard->movePiece(move);
}
