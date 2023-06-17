#pragma once
#include <TCPClient.hpp>
#include <Message.hpp>
#include <memory>
#include "Messages.hpp"

class ChessClient 
{
public:
	ChessClient();
	void run();

private:
	void handleMessage(std::shared_ptr<net::Message> message);

	std::shared_ptr<net::TCPClient> m_client;
};