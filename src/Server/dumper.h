#pragma once

#include <atomic>
#include <boost/asio.hpp>
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <bitset>
#include "../Common/logger.h"
#include "numberstore.h"

class Dumper
{
   public:
    Dumper(boost::asio::io_context& io_context, NumberStore& bitset, AsyncLogger& logger,
           int interval);

    ~Dumper();

    void stop();

   private:
    void start_timer();

    void dump();

    boost::asio::steady_timer timer_;
    NumberStore& bitset_;
    AsyncLogger& logger_;
    int interval_;
    std::atomic<bool> running_;
};