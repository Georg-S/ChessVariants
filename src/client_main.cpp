#include <iostream>
#include <memory>
#include <boost/asio.hpp>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>
#include <Client.hpp>

using namespace boost;

int main(int argc, char* argv[])
{

	system::error_code ec;
	asio::ip::tcp::endpoint endpoint(asio::ip::make_address("127.0.0.1", ec), 2345);


	return 0;
}