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
          // 停止时如果还有任务，就先处理完毕所有任务，再停止
          if (stop_ && tasks_.empty()) return;

          task = std::move(tasks_.front());
          tasks_.pop();
        }

        std::cout << type_ << " thread(" << syscall(SYS_gettid)
                  << ") start exec task." << std::endl;
        task();
      } // end while()
    });// end work thread
  }
}

ThreadPool::~ThreadPool() { Stop(); }

void ThreadPool::Stop() {
  // 设置停止标志，唤醒所有线程
  if (stop_) return;

  stop_ = true;
  condition_.notify_all();

  for (auto&& thread : threads_) {
    thread.join();
  }
}

void ThreadPool::AddTask(std::function<void()> fn) {
  // 向任务队列中添加任务，然后使用条件变量唤醒任务线程
  {
    std::lock_guard<std::mutex> lock(mtx_);
    tasks_.push(fn);
  }
  condition_.notify_one();
}