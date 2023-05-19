#pragma once
#include <memory>
#include <thread>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>

#include "Message.hpp"

namespace net 
{
	using boost::asio::ip::tcp;

	class Session : public std::enable_shared_from_this<Session>
	{
	public:
		Session(tcp::socket&& socket, size_t id);
		void start();
		void do_read();
		void do_write(std::string message);

		size_t m_id;
		tcp::socket m_socket;
		static constexpr size_t m_max_length = 1024;
		char m_buffer[m_max_length] = {};
	};
}