#include "Utility.hpp"

using namespace net;

size_t net::MessageQueue::getSize() const
{
	std::scoped_lock lock(m_mut);
	return m_messages.size();
}

size_t net::MessageQueue::addMessage(std::shared_ptr<Message> message)
{
	std::scoped_lock lock(m_mut);
	m_messages.emplace_back(message);
	return m_messages.size();
}

std::shared_ptr<Message> net::MessageQueue::getFront()
{
	std::scoped_lock lock(m_mut);

	if (m_messages.empty())
		return nullptr;
	return m_messages.front();
}

std::shared_ptr<Message> net::MessageQueue::getAndRemoveFirstMessage()
{
	std::scoped_lock lock(m_mut);
	if (m_messages.empty())
		return nullptr;

	auto message = m_messages.front();
	m_messages.pop_front();

	return message;
}

size_t net::MessageQueue::popFront()
{
	std::scoped_lock lock(m_mut);

	if (!m_messages.empty())
		m_messages.pop_front();

	return m_messages.size();
}

bool net::MessageQueue::empty() const
{
	std::scoped_lock lock(m_mut);
	return m_messages.empty();
}

net::Session::Session(std::shared_ptr<tcp::socket> socket, MessageQueue* messageQueue)
	: m_socket(socket)
	, m_id(SERVER)
	, m_messageQueue(messageQueue)
{
}

net::Session::Session(std::shared_ptr<tcp::socket> socket, uint32_t id, MessageQueue* messageQueue)
	: m_socket(socket)
	, m_id(id)
	, m_messageQueue(messageQueue)
{
}

net::Session::~Session()
{
	disconnect();
}

void net::Session::start()
{
	readHeader();
}

void net::Session::readHeader()
{
	m_currentMessageHeader = {};
	auto self(shared_from_this());
	boost::asio::async_read(*m_socket,
		boost::asio::buffer(&m_currentMessageHeader, Message::headerSize()),
		[self](boost::system::error_code ec, size_t bytesRead)
		{
			if (ec)
			{
				std::cout << "Error reading message header ... closing socket: " << ec.what();
				self->m_socket->close();
				return;
			}

			self->readBody();
		});
}

void net::Session::readBody()
{
	auto resMessage = std::make_shared<Message>(m_currentMessageHeader);

	auto self(shared_from_this());
	boost::asio::async_read(*m_socket,
		boost::asio::buffer(resMessage->getBodyStart(), resMessage->bodySize()),
		[self, resMessage](boost::system::error_code ec, std::size_t bytesRead)
		{
			if (ec)
			{
				std::cout << "Error reading message body ... closing socket: " << ec.what();
				self->m_socket->close();
				return;
			}

			self->m_messageQueue->addMessage(resMessage);
			self->readHeader();
		});
}

void net::Session::disconnect()
{
	if (m_socket->is_open())
		m_socket->close();
}

void net::Session::writeMessage(std::shared_ptr<Message> message, std::function<void(boost::system::error_code, std::size_t)> callBack)
{
	boost::asio::async_write(*m_socket, boost::asio::buffer(message->getMessageStart(), message->messageSize()), callBack);
}

bool net::Session::isConnected() const
{
	return m_socket->is_open();
}
