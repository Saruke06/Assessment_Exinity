#include "server.h"

Server::Server(boost::asio::io_context& io_context, unsigned short port, AsyncLogger& logger,
           std::function<void(tcp::socket, std::atomic<bool>&)> handler)
        : io_context_(io_context),
          acceptor_(io_context, tcp::endpoint(tcp::v4(), port)),
          logger_(logger),
          handler_(std::move(handler)),
          running_(true)
    {
        logger_.log("Server started on port " + std::to_string(port));
        accept_thread_ = std::thread(&Server::accept_connections, this);
    }

    Server::~Server()
    {
        stop();
    }

    void Server::stop()
    {
        if (!running_) return;
        running_ = false;
        acceptor_.close();
        if (accept_thread_.joinable())
        {
            accept_thread_.join();
        }
        for (auto& thread : client_threads_)
        {
            if (thread.joinable())
            {
                thread.join();
            }
        }
        logger_.log("Server stopped.");
    }

    void Server::accept_connections()
    {
        while (running_)
        {
            tcp::socket socket(io_context_);
            boost::system::error_code ec;
            acceptor_.accept(socket, ec);
            if (!ec && running_)
            {
                logger_.log("Accepted connection from " +
                            socket.remote_endpoint().address().to_string());
                client_threads_.emplace_back([this, sock = std::move(socket)]() mutable {
                    handler_(std::move(sock), running_);
                });
            }
        }
    }