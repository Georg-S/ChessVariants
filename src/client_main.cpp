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

	while (true) 
	{
		if (client->getMessageCount() > 0)
		{
			auto msg = client->getFirstMessage();
			client->popFrontMessage();

			std::string strMessage = std::string((const char*)msg->getBodyStart(), msg->bodySize());
			std::cout << strMessage << std::endl;
		}
		else 
		{
			Sleep(10);
		}
	}

	return 0;
}