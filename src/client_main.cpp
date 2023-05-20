#include <iostream>
#include <memory>
#include <boost/asio.hpp>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>
#include <TCPClient.hpp>

using namespace boost;

static void sendMessage(std::shared_ptr<net::TCPClient> client) 
{
	while (true) 
	{
		std::string myStr;
		std::cin >> myStr;
		auto outMessage = std::make_shared<net::Message>(static_cast<uint32_t>(0), static_cast<uint32_t>(0), (void*)myStr.c_str(), static_cast<uint32_t>(myStr.size() + 1));

		client->sendMessage(outMessage);
	}
}

int main(int argc, char* argv[])
{
	auto client = std::make_shared<net::TCPClient>("127.0.0.1", 2345);
	client->connect();
	client->run();

	auto inputThread = std::thread([client]() {sendMessage(client); });

	while (true) 
	{
		auto message = client->getAndRemoveFirstMessage();
		if (message)
		{
			std::string strMessage = std::string((const char*)message->getBodyStart(), message->bodySize());
			std::cout << strMessage << std::endl;
		}
	}

	inputThread.join();

	return 0;
}