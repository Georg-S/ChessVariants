#include "TCPServer.hpp"

net::TCPServer::TCPServer(std::string ip, uint16_t port)
{
	boost::system::error_code ec;
	m_acceptor = std::make_unique<tcp::acceptor>(m_context, tcp::endpoint(boost::asio::ip::make_address(ip, ec), port));
	if (ec)
		std::cout << "Error creating acceptor: " << ec.what();

	acceptConnection();
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

size_t net::TCPServer::getCountOfConnectedClients() const
{
	return m_sessions.size();
}

void net::TCPServer::sendMessage(std::shared_ptr<Message> message)
{
	auto size = m_outMessages.addMessage(message);
	if (size == 1)
		sendMessage();
}

void net::TCPServer::sendMessage()
{
	assert(!m_outMessages.empty());
	auto message = m_outMessages.getFront();

	if (message->header.id == SERVER)
		std::cout << "Message sent from server to server ... do nothing" << std::endl;
	else
		writeMessageToClient(message);
}

void net::TCPServer::writeMessageToClient(std::shared_ptr<Message> message)
{
	for (auto& session : m_sessions)
	{
		if (session->m_id == message->header.id)
		{
			sendMessage(message, session.get());
			return;
		}
	}
	std::cout << "Message could not be sent, client not found " << std::endl;
}

void net::TCPServer::sendMessage(std::shared_ptr<Message> message, Session* session)
{
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

	session->writeMessage(message, callback);
}

void net::TCPServer::acceptConnection()
{
	m_acceptor->async_accept(
		[this](boost::system::error_code ec, tcp::socket socket)
		{
			std::shared_ptr<tcp::socket> socketPtr = std::make_shared<tcp::socket>(std::move(socket));

			if (ec)
			{
				std::cout << "Connection failed with error code: " << ec.what() << std::endl;
				acceptConnection();
				return;
			}

			std::cout << m_sessionId << " connected to server" << std::endl;
			m_sessions.emplace_back(std::make_shared<Session>(socketPtr, m_sessionId++, &this->m_inMessages));
			m_sessions.back()->readHeader();

			acceptConnection();
		});
}
