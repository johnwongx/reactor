/*
线程池: 维护一个任务队列，运行过程中，多个线程尝试从任务队列中取出任务，
取出后运行任务。
*/
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
  ThreadPool(size_t threadNum, const std::string& type);
  ~ThreadPool();

  void Stop();

  void AddTask(std::function<void()> fn);

  size_t Size() const { return threads_.size(); }
  bool Empty() const { return threads_.empty(); }

 private:
  std::vector<std::thread> threads_;
  std::mutex mtx_;
  std::condition_variable condition_;
  std::queue<std::function<void()>> tasks_;
  std::atomic_bool stop_;

  std::string type_;
};