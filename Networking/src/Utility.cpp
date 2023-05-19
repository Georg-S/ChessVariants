#include "Utility.hpp"

net::Session::Session(tcp::socket&& socket, size_t id)
	: m_socket(std::move(socket))
	, m_id(id)
{
}

void net::Session::start()
{
	do_read();
}

void net::Session::do_read()
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

void net::Session::do_write(std::string message) 
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