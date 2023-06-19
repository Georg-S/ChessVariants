#pragma once
#include <cstdint>

#include <Message.hpp>
#include <GameModes/Game.hpp>

enum class MESSAGETYPE : uint32_t
{
	INIT_GAME = net::SYSTEM_MESSAGES_END,
	FEN_STRING,
	START_GAME,
	MAKE_MOVE,
	GAMESTATE_UPDATE,
	PROMOTION_POSITION,
};

struct InitMessageData
{
	chess::PieceColor playerColor;
	chess::GAME_MODES gameMode;
};