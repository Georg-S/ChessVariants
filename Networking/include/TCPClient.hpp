#pragma once
#include <cstdint>
#include <memory>
#include <thread>
#include <iostream>
#include <deque>
#include <mutex>
#include <boost/asio.hpp>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>

#include "Utility.hpp"
#include "Message.hpp"

namespace net
{
	class TCPClient : public std::enable_shared_from_this<TCPClient>
	{
	public:
		TCPClient(std::string ipAddress, uint16_t port);
		void connect();
		virtual ~TCPClient() = default;
		void run();
		void stop();

	private:
		void do_connect(const tcp::resolver::results_type& endpoints);
		void readHeader();
		void readBody();

		std::string m_ip;
		std::thread m_thread;
		uint16_t m_port;
		boost::asio::io_context m_context;
		tcp::socket m_socket;
		Message::Header m_currentMessageHeader = {};
		std::deque <std::shared_ptr<Message>> m_inMessages;
		std::mutex m_inMessagesMut;
	};
}
