#include "ThreadPool.h"

#include <sys/syscall.h>
#include <unistd.h>

#include <iostream>

ThreadPool::ThreadPool(size_t threadNum, const std::string& type)
    : stop_(false), type_(type) {
  for (size_t i = 0; i < threadNum; i++) {
    threads_.emplace_back([&] {
      while (true) {
        std::function<void()> task;

        {
          // 取出一个任务
          std::unique_lock<std::mutex> lock(mtx_);

          if (tasks_.empty()) {
            condition_.wait(lock, [&] { return stop_ || !tasks_.empty(); });
          }
          if (stop_ && tasks_.empty()) break;

          task = std::move(tasks_.front());
          tasks_.pop();
        }

        std::cout << type_ << " thread(" << syscall(SYS_gettid)
                  << ") start exec task." << std::endl;
        task();
      }
    });
  }
}

ThreadPool::~ThreadPool() {
  stop_ = true;
  condition_.notify_all();

  for (auto&& thread : threads_) {
    thread.join();
  }
}

void ThreadPool::AddTask(std::function<void()> fn) {
  {
    std::lock_guard<std::mutex> lock(mtx_);
    tasks_.push(fn);
  }
  condition_.notify_one();
}