#include "Utility.hpp"

using namespace net;

net::Session::Session(std::shared_ptr<tcp::socket> socket)
	: m_socket(socket)
	, m_id(SERVER)
{
}

net::Session::Session(std::shared_ptr<tcp::socket> socket, uint32_t id)
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
