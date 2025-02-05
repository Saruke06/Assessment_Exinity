#pragma once

#include <atomic>
#include <boost/asio.hpp>
#include <functional>
#include <thread>
#include <vector>
#include "../Common/logger.h"

using boost::asio::ip::tcp;

class Server
{
   public:
    Server(boost::asio::io_context& io_context, unsigned short port, AsyncLogger& logger,
           std::function<void(tcp::socket, std::atomic<bool>&)> handler);

    ~Server();

    void stop();

   private:
    void accept_connections();

    boost::asio::io_context& io_context_;
    tcp::acceptor acceptor_;
    AsyncLogger& logger_;
    std::function<void(tcp::socket, std::atomic<bool>&)> handler_;
    std::atomic<bool> running_;
    std::vector<std::thread> client_threads_;
    std::thread accept_thread_;
};