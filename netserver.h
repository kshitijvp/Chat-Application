#pragma once
#include <iostream>
#include "netcommon.h"
#include "tsqueue.h"
#include "netmessage.h"
#include "netconnection.h"

struct message tempMsg;

class Server
{
public:
	Server(uint16_t port)
			: acceptor(asioContext, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
	{}

	~Server()
	{
		Stop();
	}
	
public:
	bool Start()
	{
		try
		{
			AsyncAcceptClientConnection();
			thrContext = std::thread([this]() { asioContext.run(); });
		}
		catch (const std::exception& ec)
		{
			std::cout << "Exception: " << ec.what() << std::endl;
			return false;		
		}

		std::cout << "Server Started \n" << std::endl;
		return true;
	}
	void Stop()
	{
		asioContext.stop();
		if (thrContext.joinable()) thrContext.join();
		std::cout << "Server shut down \n" << std::endl;
	}
	void AsyncAcceptClientConnection() {
		acceptor.async_accept(
			[this](std::error_code ec, boost::asio::ip::tcp::socket socket) {
				if (!ec) {
					std::cout << "Accepted Connection from " << socket.remote_endpoint() << "\n" << std::endl;
					std::shared_ptr<Connection> conn = std::make_shared<Connection>(Connection::Owner::Server, asioContext, std::move(socket));
					conn->listen();
					connectionDequeue.push_back(conn);
				}
				else
					std::cout << "Error while connecting \n" << std::endl;

				AsyncAcceptClientConnection();
			});
	}

	void Update(size_t nMaxMessages = -1) {
		for (auto& connection : connectionDequeue) {
			if (!connection->GetIncomingMessageDequeue().empty()) {
				auto& msg = connection->GetIncomingMessageDequeue().front();
				MessageAll(msg, connection);
				connection->GetIncomingMessageDequeue().pop_front();
			}
		}
	}
	void MessageAll(struct message msg, std::shared_ptr<Connection> pIgnoreClient) {
		for (auto& connection : connectionDequeue) {
			if (connection != pIgnoreClient) {
				connection->Send(msg);
				std::cout << "Sent to client: " << msg.msg << std::endl;
			}
		}
		std::cout << "Sent to all clients: " << msg.msg << std::endl;
	}
protected:
	boost::asio::io_context asioContext;
	boost::asio::ip::tcp::endpoint endpoint;
	std::thread thrContext;
	boost::asio::ip::tcp::acceptor acceptor;
	std::deque<std::shared_ptr<Connection>> connectionDequeue;
	// Even the order of declaration of this
	// member fields is VERY IMP
	// Switching order causes SegFaults
	// As Destruction order is opposite of
	// declaration order.


};
