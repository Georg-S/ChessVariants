#include "GameLogic.hpp"

#include <cassert>
#include <optional>

using namespace chess;

bool chess::isKing(const Piece* piece)
{
    return tolower(piece->getFenCharacter()) == 'k';
}

bool chess::isPawn(const Piece* piece)
{
    return tolower(piece->getFenCharacter()) == 'p';
}

bool chess::isIndexInsideBoardBoundaries(const Position& position)
{
    if (position.x < 0 || position.x >= BOARD_WIDTH)
        return false;
    if (position.y < 0 || position.y >= BOARD_HEIGHT)
        return false;
    return true;
}


PieceColor chess::getNextPlayer(PieceColor player) 
{
    assert(player != PieceColor::NONE);
    if (player == PieceColor::WHITE)
        return PieceColor::BLACK;
    else if (player == PieceColor::BLACK)
        return PieceColor::WHITE;

    return PieceColor::NONE;
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

bool chess::isMoveInBoardRange(const Move& move) 
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

bool chess::isGameOver(const Board& board, PieceColor color) 
{
    return getAllPossibleMoves(board, color).empty();
}

bool chess::isCheckMate(const Board& board, PieceColor color) 
{
    return isGameOver(board, color) && isCheck(board, color);
}

bool chess::isStaleMate(const Board& board, PieceColor color) 
{
    return isGameOver(board, color) && !isCheck(board, color);
}

std::vector<Move> chess::getAllPossibleMoves(const Board& board, PieceColor color) 
{
    std::vector<Move> allPossibleMoves;

    Position pos = { 0, 0 };
    for (pos.x = 0; pos.x < BOARD_WIDTH; pos.x++)
    {
        for (pos.y = 0; pos.y < BOARD_HEIGHT; pos.y++)
        {
            auto piece = board[pos];
            if (!piece || (piece->getColor() != color))
                continue;

            auto possibleMoves = getAllPossibleMoves(board, pos);
            allPossibleMoves.insert(allPossibleMoves.end(), std::make_move_iterator(possibleMoves.begin()),
                std::make_move_iterator(possibleMoves.end()));
        }
    }

    return allPossibleMoves;
}

std::vector<Move> chess::getAllPossibleMoves(const Board& board, const Position& piecePosition)
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

static std::optional<Position> getPromotionPawnPosition(const Board& board) 
{
    Position pos = { 0, 0 };
    for (pos.x = 0; pos.x < BOARD_WIDTH; pos.x++)
    {
        for (pos.y = 0; pos.y < BOARD_HEIGHT; pos.y += 7)
        {
            auto piece = board[pos];
            if (!piece)
                continue;

            // Those two position checks are useless for normal chess, however it is required for swap chess
            if (piece->getColor() == PieceColor::WHITE && (pos.y == 7))
                continue;
            if (piece->getColor() == PieceColor::BLACK && (pos.y == 0))
                continue;
            
            if (isPawn(piece)) 
                return pos;
        }
    }
    return std::nullopt;
}

std::optional<PieceColor> chess::getPromotionSelectionColor(const Board& board) 
{
    auto pos = getPromotionPawnPosition(board);
    if (pos)
        return board[*pos]->getColor();
    return std::nullopt;
}

std::optional<Position> chess::getKingPosition(const Board& board, PieceColor color)
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

void chess::executePromotion(Board* inOutBoard, char selectedFenCharPiece) 
{
    auto pos = getPromotionPawnPosition(*inOutBoard);
    assert(pos);
    inOutBoard->replacePiece(*pos, selectedFenCharPiece);
}

Move chess::getCastlingTowerMove(const Move& kingMove) 
{
    auto diff = kingMove.to - kingMove.from;

    Position direction = { -1, 0 };
    if (diff.x > 0)
        direction.x = 1;

    Position towerPos = {};
    towerPos.y = kingMove.from.y;

    if (direction.x > 0)
        towerPos.x = 7;
    else
        towerPos.x = 0;

    return Move{ towerPos, kingMove.to - direction };
}

bool chess::isCastlingPossible(const Board& board, const Move& move, bool checkForCheck)
{
    if (!isKing(board[move.from]))
        return false;

    const auto diff = move.to - move.from;
    const auto absDiff = abs(diff);
    Position direction = { -1, 0 };
    if (diff.x > 0)
        direction.x = 1;

    if ((absDiff.x != 2) || (absDiff.y != 0))
        return false;

    const auto castlingTowerMove = getCastlingTowerMove(move);
    if (!board.castlingPossible(castlingTowerMove.from))
        return false;

    if (board.isOccupied(move.from + direction) || board.isOccupied(move.from + 2 * direction))
        return false;

    if (!checkForCheck)
        return true;

    // We don't check for check in the resulting position, this is done in the GameLogic
    for (Position toCheck = move.from; toCheck != move.to; toCheck += direction)
    {
        Board copyBoard = board.getDeepCopy();
        const Move moveKing = { move.from, toCheck };

        copyBoard.movePiece(moveKing);
        if (isCheck(copyBoard, toCheck))
            return false;
    }

    return true;
}
