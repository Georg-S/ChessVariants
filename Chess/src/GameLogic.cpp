#include "GameLogic.hpp"

bool chess::directMovePossible(const Board& board, const Move& move)
{
    if (board[move.to] && (board[move.from]->getColor() == board[move.to]->getColor()))
        return false;

    auto direction = move.to - move.from;

    if (direction.x && direction.y) 
    {
        if (abs(direction.x) != abs(direction.y))
            return false; // Piece does not move vertical, horizontal or diagonal.
    }

    if (direction.x)
        direction.x /= abs(direction.x);
    if (direction.y)
        direction.y /= abs(direction.y);

    Position currentPos = move.from;
    currentPos += direction;

    while (currentPos != move.to) 
    {
        if (board[currentPos])
            return false;

        currentPos += direction;
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
