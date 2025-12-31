#pragma once
#include "netmessage.h"
#include "netcommon.h"
#include "tsqueue.h"
#include <vector>
#include <memory>
#include <iostream>

class Connection : public std::enable_shared_from_this<Connection>
{
public:
    enum class Owner {
        Server,
        Client,
    };

    Connection(Owner parent, boost::asio::io_context& asioContext, boost::asio::ip::tcp::socket socket)
        : asioContext(asioContext), socket(std::move(socket)), parent(parent)
    {
        // Reserve memory for the read buffer
        m_readBuffer.resize(1024);
    }

    virtual ~Connection() 
    {}

public:
    tsqueue<struct message>& GetIncomingMessageDequeue() {
        return incomingMessageDequeue;
    }

    void ConnectToServer(boost::asio::ip::tcp::resolver::results_type& endpoints) {
        if (parent == Owner::Client) {
            boost::asio::async_connect(socket, endpoints,
                [this](std::error_code ec, boost::asio::ip::tcp::endpoint endpoint)
                {
                    if (!ec) {
                        std::cout << "Connected to Server \n" << std::endl;
                        listen();
                    }
                    else
                        std::cout << "Error occurred while connecting to server \n" << std::endl;
                });
        }
    }

    bool isConnected() {
        return socket.is_open();
    }

    void Send(const struct message& m) {
        boost::asio::post(asioContext, [this, m]() {
            bool bWritingMessage = !outgoingMessageDequeue.empty();
            outgoingMessageDequeue.push_back(m);
            if (!bWritingMessage) {
                WriteToSocket();
            }
        });
    }

    void listen() {
        // Use a member variable for the read buffer as
        // it needs to persist until the async operation completes.
        // Since async_read_some is non-blocking, we cannot use a
        // local variable here as it will get destroyed when this function exits.
        // So create a new member variable.
        socket.async_read_some(boost::asio::buffer(m_readBuffer.data(), m_readBuffer.size()),
            [this](std::error_code ec, std::size_t length) {
                if (!ec) {
                    // Create a string from the actual bytes read
                    std::string receivedData(m_readBuffer.data(), length);
                    std::cout << "Received: " << receivedData << std::endl;
                    AddtoIncomingMessageDeque(receivedData);
                    listen();
                }
                else {
                    std::cout << "Connection Lost: " << ec.message() << std::endl;
                    socket.close();
                }
            });
    }

    void AddtoIncomingMessageDeque(const std::string& msg) {
        struct message m;
        m.msg = msg;
        m.type = MessageType::MessageAll;
        incomingMessageDequeue.push_back(m);
    }

protected:
    void WriteToSocket() {
        if (outgoingMessageDequeue.empty()) return;

        boost::asio::async_write(socket, 
            boost::asio::buffer(outgoingMessageDequeue.front().msg.data(), outgoingMessageDequeue.front().msg.size()),
            [this](std::error_code ec, std::size_t length) {
                if (!ec) {
                    outgoingMessageDequeue.pop_front();
                    if (!outgoingMessageDequeue.empty()) {
                        WriteToSocket();
                    }
                }
                else {
                    std::cout << "Write Failed: " << ec.message() << std::endl;
                    socket.close();
                }
            });
    }

protected:
    boost::asio::ip::tcp::socket socket;
    boost::asio::io_context& asioContext;
    tsqueue<struct message> incomingMessageDequeue;
    tsqueue<struct message> outgoingMessageDequeue;
    Owner parent;
    std::vector<char> m_readBuffer;
};