#pragma once

#include <atomic>
#include <queue>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <condition_variable>

class AsyncLogger
{
   public:
    AsyncLogger(const std::string& filename);

    ~AsyncLogger();

    void log(const std::string& message);

   private:
    void process_log_queue();

    std::ofstream log_file_;
    std::queue<std::string> log_queue_;
    std::mutex queue_mutex_;
    std::condition_variable queue_cv_;
    std::thread worker_thread_;
    std::atomic<bool> stop_flag_;
};