#include <iostream>
#include <memory>
#include <boost/asio.hpp>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>
#include <Server.hpp>

using namespace boost;

int main(int argc, char* argv[])
{

	system::error_code ec;
	asio::ip::tcp::endpoint endpoint(asio::ip::make_address("127.0.0.1", ec), 2345);
	auto myServer = std::make_unique<net::Server>(endpoint);
	myServer->start();

	////asio::ip::tcp::endpoint endpoint(asio::ip::make_address("127.0.0.1", ec), 80);

	//asio::ip::tcp::socket socket(context);

	//socket.connect(endpoint, ec);

	//if (ec) 
	//{
	//	std::cout << "Error: " << ec.what() << std::endl;
	//}
	//else 
	//{
	//	std::cout << "Connected" << std::endl;
	//}

	//if (socket.is_open()) 
	//{
	//	std::string request =
	//		"GET /index.html HTTP/1.1\r\n"
	//		"Host: google.com\r\n"
	//		"Connection: close\r\n\r\n";

	//	socket.write_some(asio::buffer(request.data(), request.size()), ec);

	//	socket.wait(socket.wait_read);

	//	size_t bytes = socket.available();
	//	std::cout << bytes << std::endl;

	//	if (bytes > 0) 
	//	{
	//		std::vector<char> buffer(bytes);
	//		socket.read_some(asio::buffer(buffer.data(), buffer.size()), ec);

	//		for (auto c : buffer)
	//			std::cout << c;
	//	}

	//	if (ec)
	//	{
	//		std::cout << "Error: " << ec.what() << std::endl;
	//	}
	//}


	return 0;
}