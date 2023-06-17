#pragma once
#include <memory>
#include <map>

#include <TCPServer.hpp>
#include <Message.hpp>
#include <Game.hpp>

#include "Messages.hpp"

class ChessServer 
{
public:
	ChessServer();
	void run();

private:
	void handleMessage(std::shared_ptr<net::Message> message);
	void handleNewConnection(uint32_t newClientId);
	std::shared_ptr<net::TCPServer> m_server;
	static constexpr int MAX_ALLOWED_CONNECTIONS = 2;
	chess::GAME_MODES m_gameMode = chess::GAME_MODES::NORMAL;
	std::map<uint32_t, chess::PieceColor> m_connectionIdToColor;
};