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

#include "ThreadSafeQueue.hpp"
#include "Utility.hpp"
#include "Message.hpp"

namespace net
{
	using ClientMessageQueue = ThreadSafeQueue<Message>;

	class TCPClient
	{
	public:
		TCPClient(std::string ipAddress, uint16_t port);
		void connect();
		virtual ~TCPClient();
		void run();
		void stop();
		size_t getMessageCount() const;
		std::shared_ptr<Message> getFirstMessage();
		std::shared_ptr<Message> getAndRemoveFirstMessage();
		void popFrontMessage();
		void sendMessage(std::shared_ptr<Message> message);

	private:
		void connectTo(const tcp::resolver::results_type& endpoints);
		void readHeader();
		void readBody(std::shared_ptr<Message> unfinishedMessage);
		void sendMessage();

		std::string m_ip;
		std::thread m_thread;
		uint16_t m_port;
		boost::asio::io_context m_context;
		std::shared_ptr<Session> m_session;
		std::shared_ptr<tcp::socket> m_socket;
		ClientMessageQueue m_inMessages;
		ClientMessageQueue m_outMessages;
	};
}
