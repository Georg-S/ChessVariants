#include "GameLogic.hpp"

bool chess::directMovePossible(const Board& board, const Move& move)
{
    if (board[move.to] && (board[move.from]->getColor() == board[move.to]->getColor()))
        return false;

    auto directionX = move.to.x - move.from.x;
    auto directionY = move.to.y - move.from.y;

    if (directionX && directionY) 
    {
        if (abs(directionX) != abs(directionY))
            return false; // Piece does not move vertical, horizontal or diagonal.
    }

    if (directionX)
        directionX /= abs(directionX);
    if (directionY)
        directionY /= abs(directionY);

    Position currentPos = { move.from.x + directionX, move.from.y + directionY };

    while (currentPos != move.to) 
    {
        if (board[currentPos])
            return false;

        currentPos.x += directionX;
        currentPos.y += directionY;
    }

    return true;
}

std::vector<chess::Move> chess::getAllMovesPossible(const Board& board, const Position& piecePosition)
{
    // TODO complete method, check for check etc.
    auto piece = board[piecePosition];
    if (!piece)
        return {};

    auto result = std::vector<Move>();
    Position toPos = { 0, 0 };

    for (toPos.x = 0; toPos.x < BOARD_WIDTH; toPos.x++)
    {
        for (toPos.y = 0; toPos.y < BOARD_HEIGHT; toPos.y++)
        {            
            if (piecePosition == toPos)
                continue;

            Move move = { piecePosition, toPos };
            if (piece->movePossible(board, move))
                result.emplace_back(std::move(move));
        }
    }

    return result;
}
