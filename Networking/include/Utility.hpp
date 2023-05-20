#pragma once
#include <memory>
#include <functional>
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

	// A thread safe message queue
	class MessageQueue 
	{
	public:
		MessageQueue() = default;
		size_t getSize() const;
		size_t addMessage(std::shared_ptr<Message> message); // Returns the size after adding the message
		std::shared_ptr<Message> getFront();
		std::shared_ptr<Message> getAndRemoveFirstMessage(); // Return nullptr if empty
		size_t popFront();
		bool empty() const;

	private:
		mutable std::mutex m_mut;
		std::deque<std::shared_ptr<Message>> m_messages;
	};


	class Session : public std::enable_shared_from_this<Session>
	{
	public:
		Session(std::shared_ptr<tcp::socket> socket, MessageQueue* messageQueue);
		Session(std::shared_ptr<tcp::socket> socket, uint32_t id, MessageQueue* messageQueue);
		~Session();
		void start();
		void readHeader();
		void readBody();
		void disconnect();
		void writeMessage(std::shared_ptr<Message> message, std::function<void(boost::system::error_code, std::size_t)> callBack);
		bool isConnected() const;

		std::shared_ptr<tcp::socket> m_socket;
		uint32_t m_id;
		MessageQueue* m_messageQueue;
		Message::Header m_currentMessageHeader = {};
	};
}