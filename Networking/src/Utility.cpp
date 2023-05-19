#include "Utility.hpp"

using namespace net;

size_t net::MessageQueue::getSize() const
{
	std::scoped_lock lock(m_mut);
	return m_messages.size();
}

void net::MessageQueue::addMessage(std::shared_ptr<Message> message)
{
	m_messages.emplace_back(message);
}

std::shared_ptr<Message> net::MessageQueue::getFront()
{
	std::scoped_lock lock(m_mut);

	if (m_messages.empty())
		return nullptr;
	return m_messages.front();
}

void net::MessageQueue::popFront()
{
	std::scoped_lock lock(m_mut);

	if (!m_messages.empty())
		m_messages.pop_front();
}

net::Session::Session(std::shared_ptr<tcp::socket> socket, MessageQueue* messageQueue)
	: m_socket(socket)
	, m_id(SERVER)
	, m_messageQueue(messageQueue)
{
}

net::Session::Session(std::shared_ptr<tcp::socket> socket, size_t id, MessageQueue* messageQueue)
	: m_socket(socket)
	, m_id(id)
	, m_messageQueue(messageQueue)
{
}

void net::Session::start()
{
	read_header();
}

void net::Session::read_header()
{
	m_currentMessageHeader = {};
	auto self(shared_from_this());
	boost::asio::async_read(*m_socket,
		boost::asio::buffer(&m_currentMessageHeader, Message::headerSize()),
		[self](boost::system::error_code ec, size_t bytesRead)
		{
			if (ec)
			{
				std::cout << "Error reading message: " << ec.what();
				return;
			}

			self->read_body();
		});
}

void net::Session::read_body()
{
	auto resMessage = std::make_shared<Message>(m_currentMessageHeader);

	auto self(shared_from_this());
	boost::asio::async_read(*m_socket,
		boost::asio::buffer(resMessage->getBodyStart(), resMessage->bodySize()),
		[self, resMessage](boost::system::error_code ec, std::size_t bytesRead)
		{
			if (ec)
			{
				std::cout << "Error reading message: " << ec.what();
				return;
			}

			self->m_messageQueue->addMessage(resMessage);
			self->read_header();
		});
}

void net::Session::do_write(std::shared_ptr<Message> message)
{
	auto self = shared_from_this();

	boost::asio::async_write(*m_socket, boost::asio::buffer(message->getMessageStart(), message->messageSize()),
		[self](boost::system::error_code ec, size_t bytesTansferred)
		{
			if (ec)
			{
				std::cout << "Error occured while writing message: " << ec.what() << std::endl;
				return;
			}

			std::cout << "Sent " << bytesTansferred << " bytes." << std::endl;
		});
}
