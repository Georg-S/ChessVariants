#pragma once
#include <memory>

#include <TCPClient.hpp>
#include <Message.hpp>
#include <GameModes/Game.hpp>
#include "Messages.hpp"

class ChessClient 
{
public:
	ChessClient();
	void run();

private:
	void handleMessage(std::shared_ptr<net::Message> message);
	void handleGame();
	void handleGamePreparation();
	std::unique_ptr<chess::Game> createGame(chess::GAME_MODES gameMode) const;

	std::shared_ptr<net::TCPClient> m_client;
	chess::PieceColor m_playerColor = chess::PieceColor::NONE;
	chess::GAME_MODES m_gameMode = chess::GAME_MODES::NORMAL;
	bool m_runGame = false;
	bool m_runClient = true;
	std::unique_ptr<chess::Game> m_game;
};