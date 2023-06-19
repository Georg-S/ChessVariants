#pragma once
#include <memory>

#include <TCPClient.hpp>
#include <Message.hpp>
#include <Game.hpp>
#include "Messages.hpp"

class ChessClient 
{
public:
	ChessClient();
	void run();

private:
	void handleMessage(std::shared_ptr<net::Message> message);
	void handleGame();

	std::shared_ptr<net::TCPClient> m_client;
	chess::PieceColor m_playerColor = chess::PieceColor::NONE;
	chess::GAME_MODES m_gameMode = chess::GAME_MODES::NORMAL;
	bool m_runGame = false;
	chess::Game m_game;
};