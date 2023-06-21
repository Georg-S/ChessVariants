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
	std::unique_ptr<ChessServer> server;
	try 
	{
		server = std::make_unique<ChessServer>();
	}
	catch (const std::exception& e) 
	{
		std::cout << "Error creating ChessServer: " << e.what() << std::endl;
		return 0;
	}
	server->run();

	return 0;
}