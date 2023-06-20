#include "ChessClient.hpp"

#include <GameModes/Chess.hpp>

ChessClient::ChessClient()
{
	// TODO read ip from file
	std::string ip = "127.0.0.1";
	//std::string ip = "91.34.118.63";
	uint16_t port = 2345;
	m_client = std::make_shared<net::TCPClient>(ip, port);
}

void ChessClient::run()
{
	m_client->connect();
	m_client->run();

	while (true) // TODO run until disconnected
	{
		auto message = m_client->getAndRemoveFirstMessage();
		if (message)
			handleMessage(message);

		handleGame();
	}
}

void ChessClient::handleMessage(std::shared_ptr<net::Message> message)
{
	switch (MESSAGETYPE(message->header.messageType))
	{
	case MESSAGETYPE::INIT_GAME:
	{
		const InitMessageData* data = static_cast<const InitMessageData*>(message->getBodyStart());
		m_playerColor = data->playerColor;
		m_gameMode = data->gameMode;
		std::cout << "Init game Message received" << std::endl;

		return;
	}
	case MESSAGETYPE::START_GAME: 
	{
		assert(m_playerColor != chess::PieceColor::NONE);
		auto gameState = message->bodyToString();
		m_game = std::make_unique<chess::Chess>(gameState); // TODO handle different game modes
		m_game->enableRendering();
		m_runGame = true;

		std::cout << "Start game Message received" << std::endl;
		return;
	}
	case MESSAGETYPE::GAMESTATE_UPDATE:
	{
		m_game->setGameState(message->bodyToString());
		return;
	}
	case MESSAGETYPE::PREVIOUS_MOVE:
	{
		chess::Move move = *static_cast<chess::Move*>(message->getBodyStart());
		m_game->setPreviousMove(move);
		return;
	}
	default:
		std::cout << "Unrecognized message received" << std::endl;
		assert(!"Unrecognized message received");
		return;
	}
}

void ChessClient::handleGame()
{
	if (!m_runGame)
		return;

	m_game->update();
	if (m_playerColor != m_game->getCurrentPlayer())
		return;

	if (m_game->isGameOver())
		return;

	if (m_game->isInPromotion()) 
	{
		auto promotionPosition = m_game->getSelectedPromotionPosition();
		if (promotionPosition) 
		{
			auto promotionMessage = std::make_shared<net::Message>(net::SERVER, MESSAGETYPE::PROMOTION_POSITION, *promotionPosition);
			m_client->sendMessage(promotionMessage);
		}
		return;
	}

	auto selectedPos = m_game->getSelectedBoardPosition();
	if (!selectedPos)
		return;

	if (!m_game->isPieceSelected()) 
	{
		m_game->selectPiece(*selectedPos);
		return;
	}

	auto from = m_game->getSelectedPiecePosition();
	assert(from);
	chess::Move move = { *from, *selectedPos };
	m_game->deselectPiece();

	if (!m_game->isMovePossible(move)) 
	{
		return;
	}
	else 
	{
		auto makeMoveMessage = std::make_shared<net::Message>(net::SERVER, MESSAGETYPE::MAKE_MOVE, move);
		m_client->sendMessage(makeMoveMessage);
	}
}
