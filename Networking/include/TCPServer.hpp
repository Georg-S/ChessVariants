#pragma once
#include <optional>
#include <memory>
#include <map>
#include <thread>
#include <boost/asio.hpp>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>

#include "Message.hpp"
#include "Utility.hpp"

namespace net
{
	using boost::asio::ip::tcp;

	class TCPServer
	{
	public:
		TCPServer(std::string ip, uint16_t port);
		TCPServer(const TCPServer&) = delete;
		TCPServer& operator=(const TCPServer&) = delete;
		virtual ~TCPServer();
		void start();
		void stop();
		/// Set the maximum allowed connections / client, nullopt = no limit
		void setMaxAllowedConnections(std::optional<uint32_t> maxAllowedConnections);
		size_t getCountOfConnectedClients() const;
		std::shared_ptr<ServerMessage> getAndRemoveFirstMessage();
		void sendMessage(std::shared_ptr<Message> message);
		void broadcastMessage(std::shared_ptr<Message> message, std::initializer_list<uint32_t> ignoreIDs = {});

	private:
		void sendMessage();
		void sendMessage(std::shared_ptr<Message> message, std::shared_ptr<Session> session);
		void acceptConnection();
		void readHeader(std::shared_ptr<Session> session);
		void readBody(std::shared_ptr<Session> session, std::shared_ptr<ServerMessage> unfinishedMessage);
		void disconnectSession(std::shared_ptr<Session> session);

		static constexpr uint32_t SessionStartId = 2;
		mutable std::mutex m_sessionMutex;
		boost::asio::io_context m_context;
		std::map<uint32_t, std::shared_ptr<Session>> m_sessions;
		std::unique_ptr<tcp::acceptor> m_acceptor;
		std::thread m_thread;
		uint32_t m_sessionId = SessionStartId;
		std::optional<uint32_t> m_maxAllowedConnections;
		ThreadSafeQueue<ServerMessage> m_inMessages;
		ThreadSafeQueue<Message> m_outMessages;
	};
}
