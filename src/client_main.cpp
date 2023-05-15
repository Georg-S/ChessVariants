#include <iostream>
#include <memory>
#include <boost/asio.hpp>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>
#include <Client.hpp>

using namespace boost;

int main(int argc, char* argv[])
{
	auto client = std::make_shared<net::TCPClient>("127.0.0.1", 2345);
	client->connect();
	client->run();

	return 0;
}