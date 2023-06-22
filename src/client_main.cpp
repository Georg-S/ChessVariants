#include <iostream>
#include <memory>
#include <boost/asio.hpp>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>
#include <TCPClient.hpp>

#include <SDL/SDLHandler.hpp>
#include <Board.hpp>
#include <Renderer.hpp>
#include <GameLogic.hpp>
#include "ChessClient.hpp"


using namespace boost;

#undef main

int main(int argc, char* argv[])
{
	std::unique_ptr<ChessClient> client;
	try 
	{
		client = std::make_unique<ChessClient>();
	}
	catch (const std::exception& e) 
	{
		std::cout << "Client could not be created: " << e.what() << std::endl;
		return 0;
	}

	client->run();

	return 0;
}