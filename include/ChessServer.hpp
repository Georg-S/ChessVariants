#pragma once
#include <memory>
#include <map>
#include <fstream>

#include <TCPServer.hpp>
#include <Message.hpp>
#include <GameModes/Game.hpp>

#include "Messages.hpp"

class ChessServer 
{
public:
	ChessServer();
	void run();

private:
	void initGameLogging();
	void logCurrentGameState();
	void handleMessage(std::shared_ptr<net::ServerMessage> message);
	void handlePromotion(uint32_t clientId, const chess::Position& position);
	void handlePositionSelected(uint32_t clientId, const chess::Position& position);
	void handleMove(uint32_t clientId, const chess::Move& move);
	void handleNewConnection(uint32_t newClientId);
	void broadCastCurrentGameState(MESSAGETYPE messageType);
	void broadCastLastMove(const chess::Move& move);
	void startGame();
	static std::unique_ptr<chess::Game> createGame(chess::GAME_MODES gameMode);

	std::shared_ptr<net::TCPServer> m_server;
	static constexpr int MAX_ALLOWED_CONNECTIONS = 2;
	std::unique_ptr<chess::Game> m_game;
	chess::GAME_MODES m_gameMode = chess::GAME_MODES::NORMAL;
	std::map<uint32_t, chess::PieceColor> m_connectionIdToColor;
	std::ofstream m_gameLog;
	bool m_gameCanBeStarted = true;
	bool m_runServer = true;
};