#pragma once
#include <memory>
#include <thread>
#include <iostream>
#include <deque>
#include <boost/asio.hpp>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>

#include "Message.hpp"

namespace net 
{
	using boost::asio::ip::tcp;

	enum ConnectionID
	{
		SERVER = 0,
		BROADCAST = 1,
	};

	// A thread safe message queue
	class MessageQueue 
	{
	public:
		MessageQueue() = default;
		size_t getSize() const;
		void addMessage(std::shared_ptr<Message> message);
		std::shared_ptr<Message> getFront();
		void popFront();

	private:
		mutable std::mutex m_mut;
		std::deque<std::shared_ptr<Message>> m_messages;
	};


	class Session : public std::enable_shared_from_this<Session>
	{
	public:
		Session(std::shared_ptr<tcp::socket> socket, MessageQueue* messageQueue);
		Session(std::shared_ptr<tcp::socket> socket, size_t id, MessageQueue* messageQueue);
		void start();
		void read_header();
		void read_body();
		void do_write(std::shared_ptr<Message> message);

		std::shared_ptr<tcp::socket> m_socket;
		size_t m_id;
		MessageQueue* m_messageQueue;
		Message::Header m_currentMessageHeader = {};
	};
}