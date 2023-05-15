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
		Session(tcp::socket&& socket, size_t id)
			: m_socket(std::move(socket))
			, m_id(id)
		{
		}

		void start()
		{
			do_read();
		}

	public:
		void do_read()
		{
			auto self = shared_from_this();
			m_socket.async_read_some(boost::asio::buffer(m_buffer, m_max_length),
				[this, self](boost::system::error_code ec, std::size_t bytesTansferred)
				{
					if (ec) 
					{
						std::cout << "Error in Session occured: " << ec.what() << std::endl;
						return;
					}

					auto message = std::string(m_buffer, bytesTansferred);
					std::cout << "Received message: " << message << std::endl;
				});
		}

		void do_write(std::string message)
		{
			auto self = shared_from_this();

			strcpy_s(m_buffer, message.c_str());
			const size_t length = strlen(m_buffer) + 1;

			boost::asio::async_write(m_socket, boost::asio::buffer(m_buffer, std::size(m_buffer)),
				[self](boost::system::error_code ec, size_t bytesTansferred)
				{
					if (ec)
					{
						std::cout << "Error occured while writing message: " << ec.what() << std::endl;
						return;
					}

					std::cout << "Sent " << bytesTansferred << " bytes." << std::endl;
					//do_read();
				});
		}

		size_t m_id;
		tcp::socket m_socket;
		static constexpr size_t m_max_length = 1024;
		char m_buffer[m_max_length] = {};
	};

	// Derive from this class to create a server for your application
	class TCPServer
	{
	public:
		TCPServer(std::string ip, uint16_t port)
		{
			boost::system::error_code ec;
			m_acceptor = std::make_unique<tcp::acceptor>(m_context, tcp::endpoint(boost::asio::ip::make_address(ip, ec), port));
			if (ec) 
				std::cout << "Error creating acceptor: " << ec.what();

			do_accept();
		}

		virtual ~TCPServer()
		{
			stop();
		}

		void start() 
		{
			m_context.run();
		}

		void stop() 
		{
			m_context.stop();
		}

	private:
		void do_accept()
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
					m_sessions.back()->do_write("Hallo Welt!");

					do_accept();
				});
		}

		boost::asio::io_context m_context;
		std::vector<std::shared_ptr<Session>> m_sessions;
		std::unique_ptr<tcp::acceptor> m_acceptor;
		size_t m_sessionId = 1;
	};
}
