#include "GameLogic.hpp"

bool chess::directMovePossible(const Board& board, const Move& move)
{
    if (board[move.to] && (board[move.from]->getColor() == board[move.to]->getColor()))
        return false;

    auto direction = move.to - move.from;
    const auto absDirection = abs(direction);

    if (direction.x && direction.y) 
    {
        if (absDirection.x != absDirection.y)
            return false; // Piece does not move vertical, horizontal or diagonal.
    }

    if (direction.x)
        direction.x /= absDirection.x;
    if (direction.y)
        direction.y /= absDirection.y;

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

bool chess::movePossible(const Board& board, const Move& move)
{
    if (move.from == move.to)
        return false;

    auto piece = board[move.from];
    if (!piece)
        return false;

    if (board.hasSameColor(piece->getColor(), move.to))
        return false;

    return piece->movePossible(board, move);
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
            Move move = { piecePosition, toPos };
            if (movePossible(board, move))
                result.emplace_back(std::move(move));
        }
    }

    return result;
}
