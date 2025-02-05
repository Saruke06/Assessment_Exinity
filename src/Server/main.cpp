#include <boost/asio.hpp>
#include "../Common/logger.h"
#include "server.h"
#include "numberstore.h"
#include "dumper.h"

using boost::asio::ip::tcp;

void handle_client(tcp::socket socket, std::atomic<bool>& running, AsyncLogger& logger,
                   NumberStore& bitset)
{
    try
    {
        boost::asio::streambuf buffer;
        while (running)
        {
            boost::system::error_code error;
            std::size_t length = boost::asio::read_until(socket, buffer, '\n', error);

            if (error == boost::asio::error::eof || error == boost::asio::error::connection_reset)
            {
                break;
            }
            else if (error)
            {
                throw boost::system::system_error(error);
            }

            std::istream input_stream(&buffer);
            std::string line;
            std::getline(input_stream, line);

            try
            {
                unsigned int num = std::stoul(line);
                bitset.addNumber(num);
                double avg = bitset.getAvgOfSquares();
                logger.log("Received from client " +
                           socket.remote_endpoint().address().to_string() + ": " + line +
                           " | Avg: " + std::to_string(avg));
                boost::asio::write(socket, boost::asio::buffer(std::to_string(avg) + "\n"));
            }
            catch (const std::exception& e)
            {
                logger.log("Invalid input from client " +
                           socket.remote_endpoint().address().to_string() + ": " + line);
            }
        }
        logger.log("Client disconnected: " + socket.remote_endpoint().address().to_string());
    }
    catch (const std::exception& e)
    {
        logger.log("Client connection error: " + std::string(e.what()));
    }
}

void start_timer(boost::asio::steady_timer& timer, int interval, std::function<void()> callback)
{
    timer.expires_after(std::chrono::seconds(interval));
    timer.async_wait([&timer, interval, callback](const boost::system::error_code& ec) {
        if (!ec)
        {
            callback();
            start_timer(timer, interval, callback);
        }
    });
}

int main(int argc, char* argv[])
{
    try
    {
        boost::asio::io_context io_context;
        AsyncLogger logger("server.log");
        NumberStore bitset(logger);

        int interval = (argc > 1) ? std::stoi(argv[1]) : 10;
        Dumper dumper(io_context, bitset, logger, interval);

        Server server(io_context, 54321, logger,
                      [&](tcp::socket socket, std::atomic<bool>& running) {
                          handle_client(std::move(socket), running, logger, bitset);
                      });

        std::thread io_thread([&]() { io_context.run(); });

        std::cin.get();  // Ожидание ввода для завершения
        logger.log("Stopping server...");
        server.stop();
        dumper.stop();
        io_context.stop();

        if (io_thread.joinable())
        {
            io_thread.join();
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}
