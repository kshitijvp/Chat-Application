#pragma once
#include "netconnection.h"
#include "netmessage.h"
#include "netcommon.h"
#include "tsqueue.h"

class Client
{
public:
	Client()
	{}
public:
	bool Connect(const std::string& host, const uint16_t port)
	{
		try
		{
			boost::asio::ip::tcp::resolver resolver(asioContext);
			boost::asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(host, std::to_string(port));
			conn = std::make_unique<Connection>(Connection::Owner::Client, asioContext, boost::asio::ip::tcp::socket(asioContext));		
			conn->ConnectToServer(endpoints);
			thrContext = std::thread([this]() { asioContext.run(); });
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
			return false;
		}
		return true;
	}

	void disconnect()
	{
		asioContext.stop();
		if (thrContext.joinable())
			thrContext.join();
	}

	void Send(struct message& m) {
		conn->Send(m);
	}

	void listen() {
		conn->listen();
	}
protected:
	boost::asio::io_context asioContext;
	std::thread thrContext;
	std::unique_ptr<Connection> conn;

};
