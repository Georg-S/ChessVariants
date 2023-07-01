#include "ChessClient.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <GameModes/Chess.hpp>
#include <GameModes/SwapChess.hpp>
#include <GameModes/FogOfWarChess.hpp>
#include <GameModes/TrapChess.hpp>

ChessClient::ChessClient()
{
	boost::property_tree::ptree pt;
	boost::property_tree::ini_parser::read_ini("ChessClientConfig.ini", pt);
	auto ip = pt.get<std::string>("General.IP");
	auto port = pt.get<uint16_t>("General.Port");

	m_client = std::make_shared<net::TCPClient>(ip, port);
}

void ChessClient::run()
{
	m_client->run();

	while (m_runClient)
	{
		auto message = m_client->getAndRemoveFirstMessage();
		if (message)
			handleMessage(message);

		handleGame();
	}
}

void ChessClient::handleMessage(std::shared_ptr<net::Message> message)
{
	switch (net::SystemMessages(message->header.messageType)) 
	{
	case net::CONNECTION_CLOSED:
		//m_runClient = false;
		return;
	case net::NEW_CONNECTION:
		return; // Do nothing
	default:
		break; // Not a system message, continue with custom messages
	}


	switch (MESSAGETYPE(message->header.messageType))
	{
	case MESSAGETYPE::INIT_PLAYER:
	{
		const InitMessageData* data = static_cast<const InitMessageData*>(message->getBodyStart());
		m_playerColor = data->playerColor;
		m_gameMode = data->gameMode;
		std::cout << "Init game Message received" << std::endl;

		return;
	}
	case MESSAGETYPE::BOTH_PLAYER_CONNECTED:
	{
		m_game = createGame(m_gameMode);
		m_game->setGameState(message->bodyToString());
		m_game->enableRendering();
		m_runGame = true;
		break;
	}
	case MESSAGETYPE::START_GAME: 
	{
		assert(m_playerColor != chess::PieceColor::NONE);
		m_game->setGameReady(true);
		m_game->setGameState(message->bodyToString());
		std::cout << "Start game Message received" << std::endl;
		return;
	}
	case MESSAGETYPE::GAMESTATE_UPDATE:
	{
		m_game->setGameState(message->bodyToString());
		m_game->deselectPiece();
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
	if (!m_game->isGameReady()) 
	{
		handleGamePreparation();
		return;
	}

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

	if (!m_game->isMovePossible(move)) 
		return;

	auto makeMoveMessage = std::make_shared<net::Message>(net::SERVER, MESSAGETYPE::MAKE_MOVE, move);
	m_client->sendMessage(makeMoveMessage);
}

void ChessClient::handleGamePreparation()
{
	if (m_gameMode == chess::GAME_MODES::TRAP) 
	{
		auto pos = m_game->getSelectedBoardPosition();
		if (!pos)
			return;
		auto selectedBombPosition = std::make_shared<net::Message>(net::SERVER, MESSAGETYPE::POSITION_SELECTED, *pos);
		m_client->sendMessage(selectedBombPosition);
	}
}

std::unique_ptr<chess::Game> ChessClient::createGame(chess::GAME_MODES gameMode) const
{
	switch (gameMode)
	{
	case chess::GAME_MODES::NORMAL:	return std::make_unique<chess::Chess>();
	case chess::GAME_MODES::SWAP:	return std::make_unique<chess::SwapChess>();
	case chess::GAME_MODES::TRAP:	return std::make_unique<chess::TrapChess>(m_playerColor);
	case chess::GAME_MODES::FOGOFWAR: return std::make_unique<chess::FogOfWarChess>(m_playerColor);
	default:
		break;
	}
	assert(!"Unrecognized game mode");
	return nullptr;
}
