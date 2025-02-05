#pragma once

#include <random>
#include <string>
#include <thread>
#include <boost/asio.hpp>
#include "../Common/logger.h"

using boost::asio::ip::tcp;

class Client
{
   public:
    Client(const std::string& host, int port, const std::string& log_filename);

    void run();

    void stop();

    ~Client();

   private:
    std::atomic<bool> stop_flag_;
    AsyncLogger logger_;
    std::string host_;
    int port_;
    std::mutex stop_mutex_;
    std::condition_variable stop_cv_;
};