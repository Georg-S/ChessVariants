#include "TCPClient.hpp"

using namespace net;

net::TCPClient::TCPClient(std::string ipAddress, uint16_t port)
	: m_ip(std::move(ipAddress))
	, m_port(port)
	, m_socket(std::make_shared<tcp::socket>(m_context))
{
}

void net::TCPClient::connect()
{
	auto resolver = tcp::resolver(m_context);
	auto endPoints = resolver.resolve(m_ip, std::to_string(m_port));
	connectTo(endPoints);
}

void net::TCPClient::run()
{
	m_thread = std::thread([this]() {m_context.run(); });
}

void net::TCPClient::stop()
{
	m_context.stop();
	if (m_thread.joinable())
		m_thread.join();
	std::cout << "Client stopped" << std::endl;
}

size_t net::TCPClient::getMessageCount() const
{
	return m_inMessages.getSize();
}

std::shared_ptr<Message> net::TCPClient::getFirstMessage()
{
	return m_inMessages.getFront();
}

void net::TCPClient::popFrontMessage()
{
	m_inMessages.popFront();
}

void net::TCPClient::sendMessage(std::shared_ptr<Message> message)
{
	auto size = m_outMessages.addMessage(message);
	if (size == 1)
		sendMessage();
}

void net::TCPClient::sendMessage()
{
	assert(!m_outMessages.empty());
	auto message = m_outMessages.getFront();

	auto callback = [this](boost::system::error_code ec, std::size_t bytesWritten)
	{
		if (ec)
		{
			std::cout << "Error occured while writing message: " << ec.what() << std::endl;
			sendMessage(); // TODO maybe handle the error somehow? Currently try to send message again
			return;
		}
		std::cout << "Sent " << bytesWritten << " bytes." << std::endl;

		auto newSize = m_outMessages.popFront();
		if (newSize > 0)
			sendMessage();
	};

	m_session->writeMessage(message, callback);
}

void net::TCPClient::connectTo(const tcp::resolver::results_type& endpoints)
{
	auto self(shared_from_this());
	boost::asio::async_connect(*m_socket, endpoints,
		[self] (boost::system::error_code ec, tcp::endpoint tcpEndpoint) mutable
		{
			if (ec)
			{
				std::cout << "Connecting to server failed: " << ec.what();
				return;
			}

			std::cout << "Connected to server" << std::endl;

			self->m_session = std::make_shared<Session>(self->m_socket, &self->m_inMessages);
			self->m_session->readHeader();
		});
}
