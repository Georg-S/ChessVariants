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

	for (auto& [id, session] : m_sessions)
		session->disconnect();
	m_sessions.clear();

	if (m_thread.joinable())
		m_thread.join();

	std::cout << "Server stopped" << std::endl;
}

void net::TCPServer::setMaxAllowedConnections(uint32_t maxAllowedConnections)
{
	m_maxAllowedConnections = maxAllowedConnections;
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
		boost::asio::post(m_context, [this]() {sendMessage(); });
}

void net::TCPServer::broadcastMessage(std::shared_ptr<Message> message, std::initializer_list<uint32_t> ignoreIDs)
{
	boost::asio::post(m_context, [this, ignoreIDs, message]()
		{
			for (auto& [id, session] : m_sessions)
			{
				bool ignoreMessage = false;
				for (auto reference : ignoreIDs)
				{
					if (reference == session->m_id)
					{
						ignoreMessage = true;
						break;
					}
				}
				if (ignoreMessage)
					continue;

				auto newMessage = std::make_shared<Message>(*message);
				newMessage->setToID(session->m_id);
				sendMessage(newMessage);
			}
		}
	);
}

void net::TCPServer::sendMessage()
{
	assert(!m_outMessages.empty());
	auto message = m_outMessages.getFront();

	if (message->header.toID == SERVER)
		std::cout << "Message sent from server to server ... do nothing" << std::endl;
	else
		writeMessageToClient(message);
}

void net::TCPServer::writeMessageToClient(std::shared_ptr<Message> message)
{
	auto iter = m_sessions.find(message->header.toID);
	if (iter == m_sessions.end())
	{
		std::cout << "Message could not be sent, client not found " << std::endl;
		return;
	}

	sendMessage(message, iter->second);
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
			cleanupConnection();
		}
		else
		{
			std::cout << "Sent " << bytesWritten << " bytes." << std::endl;
		}

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

			if (m_maxAllowedConnections && (m_sessions.size() >= *m_maxAllowedConnections)) 
			{
				std::cout << "Connection denied, because maximum size of allowed clients is reached" << std::endl;
				socketPtr->close();
				acceptConnection(); // Still allow further connections, because a client might disconnect and someone else can take its place
				return;
			}

			std::cout << m_sessionId << " connected to server" << std::endl;

			auto newSession = std::make_shared<Session>(socketPtr, m_sessionId, &this->m_inMessages);
			m_sessions[m_sessionId] = newSession;
			newSession->readHeader();
			++m_sessionId;

			acceptConnection();
		});
}

void net::TCPServer::cleanupConnection()
{
	boost::asio::post(m_context, [this]()
		{
			for (auto it = m_sessions.begin(); it != m_sessions.end();)
			{
				if (!it->second->isConnected())
					it = m_sessions.erase(it);
				else
					++it;
			}
		});
}
