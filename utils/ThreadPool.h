/*线程池*/
#pragma once
#include <atomic>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool {
 public:
  ThreadPool(size_t threadNum);
  ~ThreadPool();

  void AddTask(std::function<void()> fn);

 private:
  std::vector<std::thread> threads_;
  std::mutex mtx_;
  std::condition_variable condition_;
  std::queue<std::function<void()>> tasks_;
  std::atomic_bool stop_;
};

typedef std::shared_ptr<ThreadPool> ThreadPoolPtr;