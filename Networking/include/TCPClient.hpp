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
		size_t getMessageCount() const;
		std::shared_ptr<Message> getFirstMessage();
		void popFrontMessage();

	private:
		void connectTo(const tcp::resolver::results_type& endpoints);

		std::string m_ip;
		std::thread m_thread;
		uint16_t m_port;
		boost::asio::io_context m_context;
		std::shared_ptr<Session> m_session;
		std::shared_ptr<tcp::socket> m_socket;
		MessageQueue m_inMessages;
	};
}
