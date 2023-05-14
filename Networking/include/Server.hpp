#pragma once
#include <memory>
#include <thread>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>

namespace net
{
	using boost::asio::ip::tcp;

	class Session : public std::enable_shared_from_this<Session>
	{
	public:
		Session(tcp::socket socket, size_t id)
			: m_socket(std::move(socket))
			, m_id(id)
		{
		}

		void start()
		{
			do_read();
		}

	private:
		void do_read()
		{
			auto self = shared_from_this();
			m_socket.async_read_some(boost::asio::buffer(m_buffer, m_max_length),
				[this, self](boost::system::error_code ec, std::size_t length)
				{
					if (ec) 
					{
						std::cout << "Error in Session occured: " << ec.what() << std::endl;
						return;
					}

					auto message = std::string(m_buffer, length);
					std::cout << "Received message: " << message << std::endl;
					do_write(message);
				});
		}

		void do_write(const std::string& message)
		{
			auto self = shared_from_this();
			boost::asio::async_write(m_socket, boost::asio::buffer(message),
				[this, self, message](boost::system::error_code ec, std::size_t length)
				{
					if (ec)
					{
						std::cout << "Error occured while writing message: " << ec.what() << std::endl;
						return;
					}

					std::cout << "Sent message: " << message << std::endl;
					do_read();
				});
		}

		size_t m_id;
		tcp::socket m_socket;
		static constexpr size_t m_max_length = 1024;
		char m_buffer[m_max_length] = {};
	};

	// Derive from this class to create a server for your application
	class Server
	{
	public:
		Server(const tcp::endpoint& endpoint)
			: m_acceptor(m_context, endpoint)
		{
		}

		virtual ~Server() 
		{
			stop();
		}

		void start() 
		{
			do_accept();
			m_context.run();
		}

		void stop() 
		{
			m_context.stop();
		}

	private:
		void do_accept()
		{
			m_acceptor.async_accept(
				[this](boost::system::error_code ec, tcp::socket socket)
				{
					if (!ec)
					{
						std::cout << m_sessionId << " connected to server" << std::endl;
						m_sessions.emplace_back(std::move(socket), m_sessionId++);
					}
					else 
					{
						std::cout << "Connection failed with error code: " << ec.what() << std::endl;
					}

					do_accept();
				});
		}

		boost::asio::io_context m_context;
		std::vector<Session> m_sessions;
		tcp::acceptor m_acceptor;
		size_t m_sessionId = 1;
	};
}
