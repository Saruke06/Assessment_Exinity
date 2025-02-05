#include "client.h"

Client::Client(const std::string& host, int port, const std::string& log_filename)
    : stop_flag_(false), logger_(log_filename), host_(host), port_(port)
{
    logger_.log("Client started, writing logs to " + log_filename);
}

Client::~Client()
{
    stop();
}

void Client::stop()
{
    stop_flag_.store(true);
    stop_cv_.notify_one();
}

void Client::run() {
    try
        {
            boost::asio::io_context io_context;
            tcp::socket socket(io_context);
            socket.connect(tcp::endpoint(boost::asio::ip::make_address(host_), port_));

            logger_.log("Connected to " + host_ + ":" + std::to_string(port_));

            auto seed = std::chrono::system_clock::now().time_since_epoch().count();
            std::mt19937 gen(seed);
            std::uniform_int_distribution<int> dist(0, 1023);

            while (true)
            {
                int random_number = dist(gen);
                std::string input = std::to_string(random_number);

                boost::asio::write(socket, boost::asio::buffer(input + "\n"));

                boost::asio::streambuf response;
                boost::asio::read_until(socket, response, '\n');

                std::istream response_stream(&response);
                std::string response_message;
                std::getline(response_stream, response_message);

                logger_.log("Sent: " + input + " | Server response: " + response_message);

                std::unique_lock<std::mutex> lock(stop_mutex_);
                if (stop_cv_.wait_for(lock, std::chrono::seconds(1),
                                      [this] { return stop_flag_.load(); }))
                {
                    socket.close();
                    break;
                }
            }
        }
        catch (const std::exception& e)
        {
            if (!stop_flag_.load())
            {
                logger_.log("Error: " + std::string(e.what()));
            }
        }
        logger_.log("Client stopped.");
}