#pragma once

#include <bitset>
#include <chrono>
#include <mutex>
#include <thread>
#include <iostream>
#include "../Common/logger.h"

class NumberStore
{
   public:
    NumberStore(AsyncLogger& logger);

    void addNumber(unsigned int num);

    double getAvgOfSquares() const;

    std::bitset<1024> getBitset() const;

   private:
    mutable std::mutex mtx;
    std::bitset<1024> bitset;
    uint64_t sum_of_squares;
    int unique_count;
    std::chrono::time_point<std::chrono::steady_clock> start_time;
    AsyncLogger& logger_;
};
