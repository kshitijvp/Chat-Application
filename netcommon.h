#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <deque>
#include <chrono>
#include <unordered_map>
#include <fstream>

#include <boost/asio.hpp>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>

extern std::unordered_map<std::string, boost::asio::ip::tcp::endpoint> credential_store;
