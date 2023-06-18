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

net::TCPClient::~TCPClient()
{
	m_session->disconnect();
}

void net::TCPClient::run()
{
	m_thread = std::thread([this]() {m_context.run(); });
}

void net::TCPClient::stop()
{
	m_context.stop();
	m_session->disconnect();
	if (m_thread.joinable())
		m_thread.join();
	std::cout << "Client stopped" << std::endl;
}

size_t net::TCPClient::getMessageCount() const
{
	return m_inMessages.size();
}

std::shared_ptr<Message> net::TCPClient::getFirstMessage()
{
	return m_inMessages.front();
}

std::shared_ptr<Message> net::TCPClient::getAndRemoveFirstMessage()
{
	return m_inMessages.getAndRemoveFirstElement();
}

void net::TCPClient::popFrontMessage()
{
	m_inMessages.popFront();
}

void net::TCPClient::sendMessage(std::shared_ptr<Message> message)
{
	auto self(shared_from_this());
	auto size = m_outMessages.pushBack(message);
	if (size == 1)
		boost::asio::post(m_context, [self]() {self->sendMessage(); });
}

void net::TCPClient::connectTo(const tcp::resolver::results_type& endpoints)
{
	auto self(shared_from_this());
	boost::asio::async_connect(*m_socket, endpoints,
		[self](boost::system::error_code ec, tcp::endpoint tcpEndpoint) mutable
		{
			if (ec)
			{
				std::cout << "Connecting to server failed: " << std::endl << ec.what() << std::endl;
				return;
			}

			std::cout << "Connected to server" << std::endl;

			self->m_session = std::make_shared<Session>(self->m_socket);
			self->readHeader();
		});
}

void net::TCPClient::readHeader()
{
	auto self = TCPClient::shared_from_this();
	auto currentMessage = std::make_shared<Message>();
	boost::asio::async_read(*m_session->m_socket, boost::asio::buffer(&currentMessage->header, Message::headerSize()),
		[self, currentMessage](boost::system::error_code ec, size_t bytesRead)
		{
			if (ec)
			{
				std::cout << "Error reading message header ... closing session: " << std::endl << ec.what() << std::endl;
				self->m_session->disconnect();
				return;
			}
			currentMessage->resize();

			self->readBody(currentMessage);
		}
	);
}

void net::TCPClient::readBody(std::shared_ptr<Message> unfinishedMessage)
{
	auto self = TCPClient::shared_from_this();
	boost::asio::async_read(*m_session->m_socket, boost::asio::buffer(unfinishedMessage->getBodyStart(), unfinishedMessage->bodySize()),
		[self, unfinishedMessage](boost::system::error_code ec, size_t bytesRead)
		{
			if (ec)
			{
				std::cout << "Error reading message body ... closing session: " << std::endl << ec.what() << std::endl;
				self->m_session->disconnect();
				return;
			}
			self->m_inMessages.pushBack(unfinishedMessage);
			self->readHeader();
		});
}

void net::TCPClient::sendMessage()
{
	assert(!m_outMessages.empty());
	auto message = m_outMessages.front();

	boost::asio::async_write(*m_socket, boost::asio::buffer(message->getMessageStart(), message->messageSize()),
		[this](boost::system::error_code ec, std::size_t bytesWritten)
		{
			if (ec)
			{
				std::cout << "Error occured while writing message ... closing socket: " << std::endl << ec.what() << std::endl;
				m_session->disconnect(); // For now if we encounter an error while writing a message ... disconnect
				return;
			}
			//std::cout << "Sent " << bytesWritten << " bytes." << std::endl;

			auto newSize = m_outMessages.popFront();
			if (newSize > 0)
				sendMessage();
		});
}
