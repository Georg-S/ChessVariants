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

	private:
		void do_accept();

		boost::asio::io_context m_context;
		std::vector<std::shared_ptr<Session>> m_sessions;
		std::unique_ptr<tcp::acceptor> m_acceptor;
		size_t m_sessionId = 1;
	};
}
