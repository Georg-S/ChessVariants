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

std::shared_ptr<net::Message> net::TCPServer::getAndRemoveFirstMessage()
{
	return m_inMessages.getAndRemoveFirstMessage();
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
	for (size_t i = 0; i < m_sessions.size(); i++)
	{
		auto& session = m_sessions[i];
		if (session->m_id == message->header.id)
		{
			if (session->isConnected()) 
			{
				sendMessage(message, session);
			}
			else 
			{
				std::cout << "Connection lost to client: " << session->m_id << std::endl;
				m_sessions.erase(m_sessions.begin() + i);
			}
			return;
		}
	}
	std::cout << "Message could not be sent, client not found " << std::endl;
}

void net::TCPServer::sendMessage(std::shared_ptr<Message> message, std::shared_ptr<Session> session)
{
	auto callback = [this, session](boost::system::error_code ec, std::size_t bytesWritten)
	{
		auto newQueueSize = m_outMessages.popFront();
		if (ec)
		{
			std::cout << "Error occured while writing message ... closing session: " << ec.what() << std::endl;
			session->disconnect();
			return;
		}
		std::cout << "Sent " << bytesWritten << " bytes." << std::endl;

		if (newQueueSize > 0)
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
