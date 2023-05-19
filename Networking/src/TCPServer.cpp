#include "TCPServer.hpp"

net::TCPServer::TCPServer(std::string ip, uint16_t port)
{
	boost::system::error_code ec;
	m_acceptor = std::make_unique<tcp::acceptor>(m_context, tcp::endpoint(boost::asio::ip::make_address(ip, ec), port));
	if (ec)
		std::cout << "Error creating acceptor: " << ec.what();

	do_accept();
}

net::TCPServer::~TCPServer()
{
	stop();
}

void net::TCPServer::start()
{
	m_thread = std::thread([this]() {m_context.run(); });
}

void net::TCPServer::stop()
{
	m_context.stop();
	if (m_thread.joinable())
		m_thread.join();
	std::cout << "Server stopped" << std::endl;
}

void net::TCPServer::writeMessageToClient(std::shared_ptr<Message> message, uint32_t clientId)
{
	for (auto& session : m_sessions) 
	{
		if (session->m_id == clientId)
			session->do_write(message);
	}
}

size_t net::TCPServer::getCountOfConnectedClients() const
{
	return m_sessions.size();
}

void net::TCPServer::do_accept()
{
	m_acceptor->async_accept(
		[this](boost::system::error_code ec, tcp::socket socket)
		{
			if (ec)
			{
				std::cout << "Connection failed with error code: " << ec.what() << std::endl;
				do_accept();
				return;
			}

			std::cout << m_sessionId << " connected to server" << std::endl;
			m_sessions.emplace_back(std::make_shared<Session>(std::move(socket), m_sessionId++));
			m_sessions.back()->do_read();

			do_accept();
		});
}