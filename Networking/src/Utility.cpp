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
{
}

net::Session::Session(std::shared_ptr<tcp::socket> socket, uint32_t id, MessageQueue* messageQueue)
	: m_socket(socket)
	, m_id(id)
{
}

net::Session::~Session()
{
	disconnect();
}

void net::Session::disconnect()
{
	if (m_socket->is_open())
		m_socket->close();
}

bool net::Session::isConnected() const
{
	return m_socket->is_open();
}
