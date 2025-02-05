#include "numberstore.h"

NumberStore::NumberStore(AsyncLogger& logger)
    : sum_of_squares(0),
      unique_count(0),
      start_time(std::chrono::steady_clock::now()),
      logger_(logger)
{
}

void NumberStore::addNumber(unsigned int num)
{
    if (num >= 1024) return;

    std::lock_guard<std::mutex> lock(mtx);
    if (!bitset.test(num))
    {
        bitset.set(num);
        unique_count++;
        if (unique_count == bitset.size())
        {
            auto cur_time = std::chrono::steady_clock::now();
            logger_.log("All numbers from 0 to 1023 received in " +
                        std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
                                           cur_time - start_time)
                                           .count()) +
                        " ms");
        }
        sum_of_squares += static_cast<uint64_t>(num) * num;
    }
}

double NumberStore::getAvgOfSquares() const
{
    std::lock_guard<std::mutex> lock(mtx);
    if (unique_count == 0) return 0;
    return double(sum_of_squares) / unique_count;
}

std::bitset<1024> NumberStore::getBitset() const
{
    std::lock_guard<std::mutex> lock(mtx);
    return bitset;
}