#include "logger.h"

AsyncLogger::AsyncLogger(const std::string& filename)
    : log_file_(filename, std::ios::app),
      stop_flag_(false)
{
    if (!log_file_)
    {
        throw std::runtime_error("Failed to open log file");
    }
    worker_thread_ = std::thread([this]() { process_log_queue(); });
}

AsyncLogger::~AsyncLogger()
{
    stop_flag_.store(true);
    queue_cv_.notify_one();
    if (worker_thread_.joinable())
    {
        worker_thread_.join();
    }
}

void AsyncLogger::log(const std::string& message)
{
    {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        if (log_queue_.size() > 1000)
        {
            log_queue_.pop();
        }
        log_queue_.push(message);
    }
    queue_cv_.notify_one();
}

void AsyncLogger::process_log_queue() {
    while (true)
    {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        queue_cv_.wait_for(lock, std::chrono::seconds(5), [this] { return !log_queue_.empty() || stop_flag_; });

        while (!log_queue_.empty())
        {
            log_file_ << log_queue_.front() << '\n';
            std::cout << log_queue_.front() << std::endl;
            log_queue_.pop();
        }
        log_file_.flush();

        if (stop_flag_ && log_queue_.empty())
        {
            break;
        }
    }
}