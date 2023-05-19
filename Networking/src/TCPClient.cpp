#include "TCPClient.hpp"

using namespace net;

net::TCPClient::TCPClient(std::string ipAddress, uint16_t port)
	: m_ip(std::move(ipAddress))
	, m_port(port)
	, m_socket(m_context)
{
}

void net::TCPClient::connect()
{
	auto resolver = tcp::resolver(m_context);
	auto endPoints = resolver.resolve(m_ip, std::to_string(m_port));
	do_connect(endPoints);
}

void net::TCPClient::run()
{
	m_context.run();
}

void net::TCPClient::do_connect(const tcp::resolver::results_type& endpoints)
{
	auto self(shared_from_this());
	boost::asio::async_connect(m_socket, endpoints,
		[self](boost::system::error_code ec, tcp::endpoint socket)
		{
			if (ec)
			{
				std::cout << "Connecting to server failed: " << ec.what();
				return;
			}

			std::cout << "Connected to server" << std::endl;
			self->readMessage();
		});
}

void net::TCPClient::readMessage()
{
	auto self(shared_from_this());
	boost::asio::async_read(m_socket,
		boost::asio::buffer(m_buffer, std::size(m_buffer)),
		[self](boost::system::error_code ec, std::size_t length)
		{
			if (ec)
			{
				std::cout << "Error reading message: " << ec.what();
				return;
			}

			//std::string receivedMessage = std::string(self->m_buffer);
			std::cout << (const char*)self->m_buffer << std::endl;

			self->readMessage();
		});
}
