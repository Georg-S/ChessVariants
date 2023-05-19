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
	m_thread = std::thread([this]() {m_context.run(); });
}

void net::TCPClient::stop()
{
	m_context.stop();
	if (m_thread.joinable())
		m_thread.join();
	std::cout << "Client stopped" << std::endl;
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
			self->readHeader();
		});
}

void net::TCPClient::readHeader()
{
	m_currentMessageHeader = {};
	auto self(shared_from_this());
	boost::asio::async_read(m_socket,
		boost::asio::buffer(&m_currentMessageHeader, Message::headerSize()),
		[self](boost::system::error_code ec, size_t bytesRead)
		{
			if (ec)
			{
				std::cout << "Error reading message: " << ec.what();
				return;
			}

			self->readBody();
		});
}

void net::TCPClient::readBody()
{
	auto resMessage = std::make_shared<Message>(m_currentMessageHeader);

	auto self(shared_from_this());
	boost::asio::async_read(m_socket,
		boost::asio::buffer(resMessage->getBodyStart(), resMessage->bodySize()),
		[self, resMessage](boost::system::error_code ec, std::size_t bytesRead)
		{
			if (ec)
			{
				std::cout << "Error reading message: " << ec.what();
				return;
			}
			
			self->m_inMessagesMut.lock();
			self->m_inMessages.emplace_back(std::move(resMessage));
			self->m_inMessagesMut.unlock();

			self->readHeader();
		});
}
