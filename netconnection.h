#pragma once
#include "netmessage.h"
#include "netcommon.h"

class Connection : public std::enable_shared_from_this<Connection>
{
public:
	enum class Owner {
		Server,
		Client,
	};

	Connection(Owner parent, boost::asio::io_context& asioContext, boost::asio::ip::tcp::socket socket)
			:asioContext(asioContext), socket(std::move(socket))
			{}
	~Connection()
	{}
public:
	void ConnectToServer(boost::asio::ip::tcp::resolver::results_type& endpoints) {
		if (parent == Owner::Client) {
			boost::asio::async_connect(socket, endpoints,
					[this](std::error_code ec, boost::asio::ip::tcp::endpoint endpoint)
					{
						if (!ec)
								std::cout << "Connected to Server \n" << std::endl;
						else
							std::cout << "Error occured while connecting to server \n" << std::endl;
					});
		}
	}
	bool isConnected() {
		return socket.is_open();
	}
	void Send(struct message& m) {
		outgoingMessageDequeue.push_back(m);
		boost::asio::async_write(socket, boost::asio::buffer(outgoingMessageDequeue.back().msg, outgoingMessageDequeue.back().msg.size()),
					[this](std::error_code ec, std::size_t length) {
						if (!ec)
							std::cout << "Sent Message" << std::endl;
						else
							std::cout << "Sending message failed" << std::endl;
					});
	}

	std::string listen() {
		incomingMessageDequeue.front().msg.resize(1024);
		// Set the object to some size otherwise it won't read
		// the data as by default size is 0
		auto& buffer_ref = incomingMessageDequeue.front().msg;
		socket.async_read_some(boost::asio::buffer(buffer_ref),
			[&](std::error_code ec, std::size_t length) {
				if (!ec) {
					std::cout << "Received " << buffer_ref << std::endl;
					outgoingMessageDequeue.front().msg = buffer_ref;
					outgoingMessageDequeue.front().type = MessageType::MessageAll;
					// if (parent == Owner::Server)
					// Send(outgoingMessageDequeue.front());
					listen();
					std::cout << "Listening for more messages..." << std::endl;
				}
				else {
					std::cout << "Read failed or Connection closed: " << ec.message() << std::endl;
				}
			});
		return buffer_ref;
	}

	void MessageAll (const std::string& str) {
		
	}

	void SendAll(std::string& msg) {

	}

protected:
	boost::asio::ip::tcp::socket socket;
	boost::asio::io_context& asioContext;
	std::deque<struct message> incomingMessageDequeue;
	std::deque<struct message> outgoingMessageDequeue;
	Owner parent = Owner::Client;
};
