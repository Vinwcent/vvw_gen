#ifndef VVW_WORKER_HPP
#define VVW_WORKER_HPP

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>

#include "vvw_gen/macros.hpp"

BEGIN_VVW_GEN_LIB_NS

class Worker {
 public:
  Worker(std::function<void(std::unique_lock<std::mutex> &lock)> threadFunction,
         std::function<bool()> hasWorkToDo);
  ~Worker();

  // Call without holding getMutex().
  void setBlock(bool isBlocked);
  void notifyWorkWasAdded();

  std::mutex &getMutex();

  // Serialize calls; call outside the worker thread, without holding getMutex().
  void shutdown();

  bool isWorking();

 private:
  std::atomic<bool> isActive_ = true;
  std::atomic<bool> isBlocked_ = false;
  std::atomic<bool> isWorking_ = false;
  std::condition_variable cv_;

  std::function<void(std::unique_lock<std::mutex> &lock)> threadFunction_;
  std::function<bool()> hasWorkToDo_;
  std::mutex workMutex_;

  // Starting the thread publishes all preceding members to threadLoop_.
  std::thread workerThread_;

  void threadLoop_();
};

END_VVW_GEN_LIB_NS

#endif
