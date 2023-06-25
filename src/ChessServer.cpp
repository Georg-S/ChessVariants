#include "ChessServer.hpp"

#include <regex>
#include <filesystem>
#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <GameModes/Chess.hpp>
#include <GameModes/SwapChess.hpp>
#include <GameModes/FogOfWarChess.hpp>
#include <GameModes/TrapChess.hpp>

static int getHighestFileNumber(const std::string& path)
{
	int highestExistingNumber = -1;
	std::smatch baseMatch;
	std::error_code ec;
	for (const auto& entry : std::filesystem::directory_iterator(path, ec))
	{
		auto pathString = entry.path().string();
		if (std::regex_search(pathString, baseMatch, std::regex("\\d+")))
		{
			int iteration = std::stoi(baseMatch[0]);
			if (iteration >= highestExistingNumber)
				highestExistingNumber = iteration;
		}
	}

	return highestExistingNumber;
}

static bool doesGameModeRequirePreparation(chess::GAME_MODES gameMode) 
{
	if (gameMode == chess::GAME_MODES::TRAP)
		return true;
	return false;
}

ChessServer::ChessServer()
{
	boost::property_tree::ptree pt;
	boost::property_tree::ini_parser::read_ini("ChessServerConfig.ini", pt);
	auto ip = pt.get<std::string>("General.IP");
	auto port = pt.get<uint16_t>("General.Port");
	auto gameModeBuf = pt.get<int>("General.GameMode");
	m_gameMode = chess::GAME_MODES(gameModeBuf);
	m_gameCanBeStarted = !doesGameModeRequirePreparation(m_gameMode);

	// "4k3/RR6/8/8/8/8/8/4K3 w - - 0 1"
	m_game = createGame(m_gameMode);
	m_server = std::make_unique<net::TCPServer>(ip, port);
	m_server->setMaxAllowedConnections(MAX_ALLOWED_CONNECTIONS);
	initGameLogging();
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

void ChessServer::initGameLogging()
{
	if (!std::filesystem::exists("GameLogs"))
		std::filesystem::create_directory("GameLogs");

	auto highestNumber = getHighestFileNumber("GameLogs/");
	++highestNumber;
	m_gameLog = std::ofstream("GameLogs/" + std::to_string(highestNumber) + "_" + chess::gameModeToString(m_gameMode) + ".txt");
}

void ChessServer::logCurrentGameState()
{
	auto currentGameState = m_game->getGameState();
	m_gameLog <<  currentGameState << std::endl;
}

void ChessServer::handleMessage(std::shared_ptr<net::ServerMessage> message)
{
	const auto messageType = message->header.messageType;

	switch (messageType)
	{
	case net::NEW_CONNECTION:
	{
		handleNewConnection(message->fromID);
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
	case MESSAGETYPE::PROMOTION_POSITION:
	{
		chess::Position position = *static_cast<const chess::Position*>(message->getBodyStart());
		handlePromotion(message->fromID, position);
		break;
	}
	case MESSAGETYPE::POSITION_SELECTED:
	{
		chess::Position position = *static_cast<const chess::Position*>(message->getBodyStart());
		handlePositionSelected(message->fromID, position);
		break;
	}
	default:
		assert(!"Unhandled message");
		break;
	}
}

void ChessServer::handlePromotion(uint32_t clientId, const chess::Position& position)
{
	auto color = m_connectionIdToColor[clientId];
	if (color != m_game->getCurrentPlayer())
		return;

	m_game->pawnPromotion(position);
	broadCastCurrentGameState(MESSAGETYPE::GAMESTATE_UPDATE);
}

void ChessServer::handlePositionSelected(uint32_t clientId, const chess::Position& position)
{
	if (m_gameMode != chess::GAME_MODES::TRAP)
		return; // Currently only trap chess uses this kind of message to prepare the game

	auto trapChess = dynamic_cast<chess::TrapChess*>(m_game.get());
	trapChess->placeBomb(position, m_connectionIdToColor[clientId]);
	broadCastCurrentGameState(MESSAGETYPE::GAMESTATE_UPDATE);
}

void ChessServer::handleMove(uint32_t clientId, const chess::Move& move)
{
	assert(m_connectionIdToColor.find(clientId) != m_connectionIdToColor.end());
	auto color = m_connectionIdToColor[clientId];
	if (color != m_game->getCurrentPlayer())
		return;

	if (!m_game->isMovePossible(move))
		return;

	m_game->makeMove(move);
	broadCastCurrentGameState(MESSAGETYPE::GAMESTATE_UPDATE);
	broadCastLastMove(move);
	logCurrentGameState();
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
		startGame();
}

void ChessServer::broadCastCurrentGameState(MESSAGETYPE messageType)
{
	auto startGameMessage = std::make_shared<net::Message>(net::BROADCAST, messageType, m_game->getGameState());
	m_server->broadcastMessage(startGameMessage);
}

void ChessServer::broadCastLastMove(const chess::Move& move)
{
	auto startGameMessage = std::make_shared<net::Message>(net::BROADCAST, MESSAGETYPE::PREVIOUS_MOVE, move);
	m_server->broadcastMessage(startGameMessage);
}

void ChessServer::startGame()
{
	broadCastCurrentGameState(MESSAGETYPE::START_GAME);
	logCurrentGameState();
}

std::unique_ptr<chess::Game> ChessServer::createGame(chess::GAME_MODES gameMode)
{
	switch (gameMode)
	{
	case chess::GAME_MODES::NORMAL:	return std::make_unique<chess::Chess>();
	case chess::GAME_MODES::SWAP:	return std::make_unique<chess::SwapChess>();
	// The server doesn't render the game therefore the color doesn't matter
	case chess::GAME_MODES::TRAP:	return std::make_unique<chess::TrapChess>(chess::PieceColor::NONE);
	// The server doesn't render the game therefore the color doesn't matter
	case chess::GAME_MODES::FOGOFWAR: return std::make_unique<chess::FogOfWarChess>(chess::PieceColor::NONE); 
	default:
		break;
	}
	assert(!"Unrecognized game mode");
	return nullptr;
}
