#pragma once
#include <memory>
#include <thread>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>

#include "Message.hpp"
#include "Utility.hpp"

namespace net
{
	using boost::asio::ip::tcp;

	class TCPServer
	{
	public:
		TCPServer(std::string ip, uint16_t port);
		virtual ~TCPServer();
		void start();
		void stop();
		size_t getCountOfConnectedClients() const;
		std::shared_ptr<Message> getAndRemoveFirstMessage();
		void sendMessage(std::shared_ptr<Message> message);
		void broadcastMessage(std::shared_ptr<Message> message);

	private:
		void sendMessage();
		void writeMessageToClient(std::shared_ptr<Message> message);
		void sendMessage(std::shared_ptr<Message> message, std::shared_ptr<Session> session);
		void acceptConnection();

		static constexpr uint32_t SessionStartId = 2;
		boost::asio::io_context m_context;
		std::vector<std::shared_ptr<Session>> m_sessions;
		std::unique_ptr<tcp::acceptor> m_acceptor;
		std::thread m_thread;
		uint32_t m_sessionId = SessionStartId;
		MessageQueue m_inMessages;
		MessageQueue m_outMessages;
	};
}
