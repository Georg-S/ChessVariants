#include "ChessServer.hpp"

ChessServer::ChessServer()
{
	std::string ip = "0.0.0.0";
	// TODO port probably should be configurable in a file
	uint16_t port = 2345;
	// TODO read in the game mode from a file
	m_gameMode = chess::GAME_MODES::NORMAL;

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

void ChessServer::handleMessage(std::shared_ptr<net::Message> message)
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

void ChessServer::handleNewConnection(uint32_t newClientId)
{
	if (m_connectionIdToColor.empty())
		m_connectionIdToColor.emplace(newClientId, chess::PieceColor::WHITE);
	else
		m_connectionIdToColor.emplace(newClientId, chess::PieceColor::BLACK);

	InitMessageData data{chess::PieceColor::WHITE, m_gameMode };
	auto newMessage = std::make_shared<net::Message>(newClientId, MESSAGETYPE::INIT_GAME, data);
	m_server->sendMessage(newMessage);
}
