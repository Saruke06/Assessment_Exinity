#include <atomic>
#include <boost/asio.hpp>
#include <chrono>
#include <condition_variable>
#include <fstream>
#include <iostream>
#include <mutex>
#include <queue>
#include <random>
#include <string>
#include <thread>

#include "../Common/logger.h"
#include "client.h"

using boost::asio::ip::tcp;

std::string generateRandomString(size_t length)
{
    static std::mt19937 gen(std::chrono::system_clock::now().time_since_epoch().count());
    static std::uniform_int_distribution<char> dist('a', 'z');

    std::string result(length, '\0');
    std::generate(result.begin(), result.end(), [] { return dist(gen); });
    return result;
}

int main()
{
    std::string log_filename = "logs/client_log_" + generateRandomString(5) + ".txt";
    Client client("127.0.0.1", 54321, log_filename);

    std::cout << "Press Enter to stop the client..." << std::endl;
    std::thread client_thread(&Client::run, &client);

    std::cin.get();

    client.stop();
    if (client_thread.joinable())
    {
        client_thread.join();
    }

    std::cout << "Program terminated." << std::endl;
    return 0;
}
