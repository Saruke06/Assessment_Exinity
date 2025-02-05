#include "dumper.h"

namespace
{

template <size_t N>
std::string bitsetToHex(const std::bitset<N>& bits)
{
    static const char hexDigits[] = "0123456789ABCDEF";
    std::string bin = bits.to_string();

    while (bin.size() % 4 != 0)
    {
        bin = "0" + bin;
    }

    std::string hex;
    for (size_t i = 0; i < bin.size(); i += 4)
    {
        int value = std::stoi(bin.substr(i, 4), nullptr, 2);
        hex += hexDigits[value];
    }

    return hex;
}

}

Dumper::Dumper(boost::asio::io_context& io_context, NumberStore& bitset, AsyncLogger& logger,
               int interval)
    : timer_(io_context), bitset_(bitset), logger_(logger), interval_(interval), running_(true)
{
    start_timer();
}

Dumper::~Dumper()
{
    stop();
}

void Dumper::stop()
{
    running_ = false;
    timer_.cancel();
    dump();
}

void Dumper::start_timer()
{
    timer_.expires_after(std::chrono::seconds(interval_));
    timer_.async_wait(
        [this](const boost::system::error_code& ec)
        {
            if (!ec && running_)
            {
                dump();
                start_timer();
            }
        });
}

void Dumper::dump()
{
    try
    {
        auto dump_data = bitset_.getBitset();
        std::ofstream file("dump.bin", std::ios::binary);
        if (!file)
        {
            throw std::runtime_error("Failed to open dump file");
        }
        std::string dump_str = dump_data.to_string();
        file.write(dump_str.c_str(), dump_str.size());
        if (!file)
        {
            throw std::runtime_error("Failed to write to dump file");
        }
        file.close();
        logger_.log("Dump file written successfully: " + bitsetToHex(dump_data));
    }
    catch (const std::exception& e)
    {
        logger_.log(std::string("Error during dump: ") + e.what());
    }
}