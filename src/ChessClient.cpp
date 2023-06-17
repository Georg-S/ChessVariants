#include "ChessClient.hpp"

static void sendMessage(std::shared_ptr<net::TCPClient> client)
{
	while (true)
	{
		std::string myStr;
		std::cin >> myStr;
		auto outMessage = std::make_shared<net::Message>(static_cast<uint32_t>(0), static_cast<uint32_t>(0), (void*)myStr.c_str(), static_cast<uint32_t>(myStr.size() + 1));

		client->sendMessage(outMessage);
	}
}

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

	auto inputThread = std::thread([this]() {sendMessage(m_client); });

	while (true) // TODO run until disconnected
	{
		auto message = m_client->getAndRemoveFirstMessage();
		if (message)
			handleMessage(message);

	}
}

void ChessClient::handleMessage(std::shared_ptr<net::Message> message)
{
	switch (MESSAGETYPE(message->header.messageType))
	{
	case MESSAGETYPE::INIT_GAME:
	{
		const InitMessageData* data = static_cast<InitMessageData*>(message->getBodyStart());

		std::cout << "Init game Message received" << std::endl;

		// TODO
		break;
	}
	default:
		std::cout << "Unrecognized Message received" << std::endl;
		break;
	}
}
