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

	// Derive from this class to create a server for your application
	class TCPServer
	{
	public:
		TCPServer(std::string ip, uint16_t port);
		virtual ~TCPServer();
		void start();
		void stop();
		void writeMessageToClient(std::shared_ptr<Message> message, uint32_t clientId);
		size_t getCountOfConnectedClients() const;

	private:
		void do_accept();

		static constexpr uint32_t SessionStartId = 1;
		boost::asio::io_context m_context;
		std::vector<std::shared_ptr<Session>> m_sessions;
		std::unique_ptr<tcp::acceptor> m_acceptor;
		std::thread m_thread;
		uint32_t m_sessionId = SessionStartId;
		MessageQueue m_inMessages;
	};
}
