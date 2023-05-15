#include <iostream>
#include <memory>
#include <boost/asio.hpp>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>
#include <Server.hpp>

using namespace boost;

int main(int argc, char* argv[])
{
	auto myServer = std::make_unique<net::TCPServer>("127.0.0.1", 2345);
	myServer->start();

	return 0;
}