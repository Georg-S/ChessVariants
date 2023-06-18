#include "ChessServer.hpp"

ChessServer::ChessServer()
{
	std::string ip = "0.0.0.0";
	// TODO port probably should be configurable in a file
	uint16_t port = 2345;
	// TODO read in the game mode from a file
	m_gameMode = chess::GAME_MODES::NORMAL;

	m_game = chess::Game(); // Todo handle different game modes etc.
	m_server = std::make_unique<net::TCPServer>(ip, port);
	m_server->setMaxAllowedConnections(MAX_ALLOWED_CONNECTIONS);
}

void ChessServer::run()
{
	m_server->start();

	while (true)
	{
		auto inMessage = m_server->getAndRemoveFirstMessage();
		if (inMessage)
			handleMessage(inMessage);

	}
}

void ChessServer::handleMessage(std::shared_ptr<net::ServerMessage> message)
{
	const auto messageType = message->header.messageType;

	switch (messageType) 
	{
	case net::NEW_CONNECTION: 
	{
		handleNewConnection(message->header.toID);
		return;
	}
	case net::CONNECTION_CLOSED:
	{
		// TODO handle connection loss
		return;
	}
	default:
		break; // Not a system message, continue with custom messages
	}

	switch (MESSAGETYPE(messageType))
	{
	case MESSAGETYPE::FEN_STRING: 
	{
		assert(!"Server shouldn't receive this message");
		break;
	}
	case MESSAGETYPE::MAKE_MOVE: 
	{
		chess::Move move = *static_cast<const chess::Move*>(message->getBodyStart());
		handleMove(message->fromID, move);
		break;
	}
	default:
		assert(!"Unhandled message");
		break;
	}


	//{
	//	m_server->broadcastMessage(inMessage);
	//	auto str = std::string(static_cast<const char*>(inMessage->getBodyStart()), inMessage->bodySize());
	//	std::cout << str << std::endl;
	//}
}

void ChessServer::handleMove(uint32_t clientId, const chess::Move& move)
{
	assert(m_connectionIdToColor.find(clientId) != m_connectionIdToColor.end());
	auto color = m_connectionIdToColor[clientId];
	if (color != m_game.getCurrentPlayer())
		return;

	if (!m_game.isMovePossible(move))
		return;

	m_game.makeMove(move);
	broadCastCurrentGameState(MESSAGETYPE::GAMESTATE_UPDATE);
}

void ChessServer::handleNewConnection(uint32_t newClientId)
{
	chess::PieceColor newConnectionColor = chess::PieceColor::WHITE;
	if (!m_connectionIdToColor.empty())
		newConnectionColor = chess::PieceColor::BLACK;

	m_connectionIdToColor.emplace(newClientId, newConnectionColor);

	InitMessageData data{ newConnectionColor, m_gameMode };
	auto initPlayerMessage = std::make_shared<net::Message>(newClientId, MESSAGETYPE::INIT_GAME, data);
	m_server->sendMessage(initPlayerMessage);

	if (m_connectionIdToColor.size() == 2) 
		broadCastCurrentGameState(MESSAGETYPE::START_GAME);
}

void ChessServer::broadCastCurrentGameState(MESSAGETYPE messageType)
{
	auto startGameMessage = std::make_shared<net::Message>(net::BROADCAST, messageType, m_game.getFenString());
	m_server->broadcastMessage(startGameMessage);
}
