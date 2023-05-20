#include <iostream>
#include <memory>
#include <boost/asio.hpp>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>
#include <TCPClient.hpp>

using namespace boost;

int main(int argc, char* argv[])
{
	auto client = std::make_shared<net::TCPClient>("127.0.0.1", 2345);
	client->connect();
	client->run();

	std::string test = "Pong!";
	auto outMessage = std::make_shared<net::Message>(static_cast<uint32_t>(0), (void*)test.c_str(), static_cast<uint32_t>(test.size() + 1));

	while (true) 
	{
		auto message = client->getAndRemoveFirstMessage();
		if (message)
		{
			std::string strMessage = std::string((const char*)message->getBodyStart(), message->bodySize());
			std::cout << strMessage << std::endl;

			client->sendMessage(outMessage);
		}
	}

	return 0;
}