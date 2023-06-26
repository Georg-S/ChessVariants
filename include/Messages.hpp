#pragma once
#include <cstdint>

#include <Message.hpp>
#include <GameModes/Game.hpp>

enum class MESSAGETYPE : uint32_t
{
	INIT_PLAYER = net::SYSTEM_MESSAGES_END,
	BOTH_PLAYER_CONNECTED,
	FEN_STRING,
	START_GAME,
	GAME_PREPARATION_FINISHED,
	MAKE_MOVE,
	GAMESTATE_UPDATE,
	PROMOTION_POSITION,
	PREVIOUS_MOVE,
	POSITION_SELECTED,
};

struct InitMessageData
{
	chess::PieceColor playerColor;
	chess::GAME_MODES gameMode;
};