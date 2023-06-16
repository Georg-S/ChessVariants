#include "GameLogic.hpp"

#include <cassert>
#include <optional>

using namespace chess;

static bool isKing(const Piece* piece) 
{
    return tolower(piece->getFenCharacter()) == 'k';
}

static std::optional<Position> getKingPosition(const Board& board, PieceColor color) 
{
    Position pos = {};
    for (pos.x = 0; pos.x < BOARD_WIDTH; pos.x++)
    {
        for (pos.y = 0; pos.y < BOARD_HEIGHT; pos.y++)
        {
            auto piece = board[pos];
            if (!piece || color != piece->getColor())
                continue;

            if (isKing(piece))
                return pos;
        }
    }

    return std::nullopt;
}

void chess::makeMove(Board* inOutBoard, const Move& move)
{
    auto piece = (*inOutBoard)[move.from];
    if (!piece) 
    {
        assert(!"Invalid field selected");
        return;
    }

    piece->makeMove(inOutBoard, move);
}

bool chess::isCheck(const Board& board, PieceColor color)
{
    auto kingPos = getKingPosition(board, color);
    if (!kingPos) 
    {
        assert(!"King not found");
        return false;
    }

    return isCheck(board, *kingPos);
}

bool chess::isCheck(const Board& board, const Position& kingPos)
{
    auto king = board[kingPos];
    auto color = king->getColor();
    Position pos = {};
    for (pos.x = 0; pos.x < BOARD_WIDTH; pos.x++)
    {
        for (pos.y = 0; pos.y < BOARD_HEIGHT; pos.y++)
        {
            auto piece = board[pos];
            if (!piece || color == piece->getColor())
                continue;

            const Move move = { pos, kingPos };
            if (piece->movePossible(board, move))
                return true;
        }
    }

    return false;
}

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

static bool isMoveInBoardRange(const Move& move) 
{
    if (move.from.x < 0 || move.from.x >= BOARD_WIDTH)
        return false;
    if (move.from.y < 0 || move.from.y >= BOARD_HEIGHT)
        return false;
    if (move.to.x < 0 || move.to.x >= BOARD_WIDTH)
        return false;
    if (move.to.y < 0 || move.to.y >= BOARD_HEIGHT)
        return false;

    return true;
}

bool chess::isMovePossible(const Board& board, const Move& move)
{
    if (!isMoveInBoardRange(move)) 
    {
        assert(!"Move out of range");
        return false;
    }

    if (move.from == move.to)
        return false;

    auto piece = board[move.from];
    if (!piece)
        return false;

    const auto playerColor = piece->getColor();
    if (board.hasSameColor(playerColor, move.to))
        return false;

    if (!piece->movePossible(board, move))
        return false;

    auto copyBoard = board.getDeepCopy();
    makeMove(&copyBoard, move);

    return !isCheck(copyBoard, playerColor);
}

std::vector<Move> chess::getAllMovesPossible(const Board& board, const Position& piecePosition)
{
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
            if (isMovePossible(board, move))
                result.emplace_back(std::move(move));
        }
    }

    return result;
}
