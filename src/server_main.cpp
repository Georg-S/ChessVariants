#include <iostream>
#include <memory>
#include <boost/asio.hpp>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>
#include "ChessServer.hpp"

using namespace boost;

#undef main

int main(int argc, char* argv[])
{
	ChessServer server = ChessServer();
	server.run();

	return 0;
}