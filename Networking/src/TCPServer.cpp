#include "TCPServer.hpp"

net::TCPServer::TCPServer(std::string ip, uint16_t port)
{
	boost::system::error_code ec;
	m_acceptor = std::make_unique<tcp::acceptor>(m_context, tcp::endpoint(boost::asio::ip::make_address(ip, ec), port));
	if (ec)
		std::cout << "Error creating acceptor: " << std::endl << ec.what() << std::endl;

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
	std::scoped_lock lock(m_sessionMutex);
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
	boost::asio::async_write(*session->m_socket, boost::asio::buffer(message->getMessageStart(), message->messageSize()),
		[this, session](boost::system::error_code ec, std::size_t bytesWritten)
		{
			auto newQueueSize = m_outMessages.popFront();
			if (ec)
			{
				std::cout << "Error occured while writing message ... closing session: " << std::endl << ec.what() << std::endl;
				session->disconnect();
				cleanupConnection();
			}
			else
			{
				//std::cout << "Sent " << bytesWritten << " bytes." << std::endl;
			}

			if (newQueueSize > 0)
				sendMessage();
		});
}

void net::TCPServer::acceptConnection()
{
	m_acceptor->async_accept(
		[this](boost::system::error_code ec, tcp::socket socket)
		{
			std::shared_ptr<tcp::socket> socketPtr = std::make_shared<tcp::socket>(std::move(socket));

			if (ec)
			{
				std::cout << "Connection failed with error code: " << std::endl << ec.what() << std::endl;
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

			std::scoped_lock lock(m_sessionMutex);
			std::cout << m_sessionId << " connected to server" << std::endl;
			auto newSession = std::make_shared<Session>(socketPtr, m_sessionId, &this->m_inMessages);
			m_sessions[m_sessionId] = newSession;
			readHeader(newSession);

			// Create and emplace a message so the user of the server knows, when a client has connected
			auto message = std::make_shared<Message>();
			message->header.messageType = NEW_CONNECTION;
			message->header.toID = m_sessionId;
			m_inMessages.addMessage(message);

			++m_sessionId;

			acceptConnection();
		});
}

void net::TCPServer::readHeader(std::shared_ptr<Session> session)
{
	auto currentMessage = std::make_shared<Message>();
	boost::asio::async_read(*session->m_socket, boost::asio::buffer(&currentMessage->header, Message::headerSize()),
		[this, session, currentMessage](boost::system::error_code ec, size_t bytesRead)
		{
			if (ec)
			{
				std::cout << "Error reading message header ... closing session: " << std::endl << ec.what() << std::endl;
				session->disconnect();
				cleanupConnection();
				return;
			}
			currentMessage->resize();

			readBody(session, currentMessage);
		}
	);
}

void net::TCPServer::readBody(std::shared_ptr<Session> session, std::shared_ptr<Message> unfinishedMessage)
{
	boost::asio::async_read(*session->m_socket, boost::asio::buffer(unfinishedMessage->getBodyStart(), unfinishedMessage->bodySize()),
		[this, session, unfinishedMessage](boost::system::error_code ec, size_t bytesRead)
		{
			if (ec)
			{
				std::cout << "Error reading message body ... closing session: " << std::endl << ec.what() << std::endl;
				session->disconnect();
				cleanupConnection();
				return;
			}
			m_inMessages.addMessage(unfinishedMessage);

			readHeader(session);
		});
}

void net::TCPServer::cleanupConnection()
{
	boost::asio::post(m_context, [this]()
		{
			std::scoped_lock lock(m_sessionMutex);
			for (auto it = m_sessions.begin(); it != m_sessions.end();)
			{
				if (!it->second->isConnected()) 
				{
					it = m_sessions.erase(it);

					// Create and emplace a message so the user of the server knows, when a client has disconnected
					auto message = std::make_shared<Message>();
					message->header.messageType = CONNECTION_CLOSED;
					message->header.toID = m_sessionId;
					m_inMessages.addMessage(message);
				}
				else 
				{
					++it;
				}
			}
		});
}
