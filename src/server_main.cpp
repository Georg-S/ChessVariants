#include <iostream>
#include <memory>
#include <boost/asio.hpp>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>
#include <TCPServer.hpp>

using namespace boost;

int main(int argc, char* argv[])
{
	auto myServer = std::make_unique<net::TCPServer>("127.0.0.1", 2345);
	myServer->start();

	bool doOnce = true;
	std::string test = "Hallo Welt!";
	auto message = std::make_shared<net::Message>(static_cast<uint32_t>(1), (void*)test.c_str(), static_cast<uint32_t>(test.size() + 1));

	while (true) 
	{
		if (myServer->getCountOfConnectedClients() && doOnce) 
		{
			doOnce = false;
			myServer->writeMessageToClient(message, 1);
		}
	}


	return 0;
}