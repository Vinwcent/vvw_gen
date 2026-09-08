#include <gtest/gtest.h>

#include <chrono>
#include <future>

#include <vvw_gen/worker.hpp>

using namespace std::chrono_literals;

TEST(WorkerTest, usage) {
  int value = 1;
  std::promise<void> completed;
  auto completion = completed.get_future();
  vvw_gen::Worker worker(
      [&](auto &) {
        if (++value == 10) completed.set_value();
      },
      [&]() { return value > 1 && value < 10; });
  {
    std::lock_guard<std::mutex> lock(worker.getMutex());
    EXPECT_EQ(value, 1);
    value += 1;
  }
  worker.notifyWorkWasAdded();
  ASSERT_EQ(completion.wait_for(5s), std::future_status::ready);
  worker.shutdown();
  EXPECT_EQ(value, 10);
}

TEST(WorkerTest, block) {
  for (int iteration = 0; iteration < 64; ++iteration) {
    SCOPED_TRACE(iteration);
    bool pending = false;
    std::promise<void> completed;
    auto completion = completed.get_future();
    vvw_gen::Worker worker(
        [&](auto &) {
          pending = false;
          completed.set_value();
        },
        [&]() { return pending; });
    worker.setBlock(true);
    {
      std::lock_guard<std::mutex> lock(worker.getMutex());
      pending = true;
    }
    worker.notifyWorkWasAdded();
    EXPECT_EQ(completion.wait_for(0s), std::future_status::timeout);

    // Unblocking must wake pending work without another producer notification.
    worker.setBlock(false);
    ASSERT_EQ(completion.wait_for(5s), std::future_status::ready);
    worker.shutdown();
    EXPECT_FALSE(pending);
  }
}

TEST(WorkerTest, constructionWithPendingWork) {
  for (int iteration = 0; iteration < 64; ++iteration) {
    SCOPED_TRACE(iteration);
    bool pending = true;
    std::promise<void> completed;
    auto completion = completed.get_future();
    vvw_gen::Worker worker(
        [&](auto &) {
          pending = false;
          completed.set_value();
        },
        [&]() { return pending; });
    ASSERT_EQ(completion.wait_for(5s), std::future_status::ready);
  }
}

TEST(WorkerTest, immediateIdleShutdownAndDestruction) {
  for (int iteration = 0; iteration < 128; ++iteration) {
    SCOPED_TRACE(iteration);
    vvw_gen::Worker worker([](auto &) {}, []() { return false; });
    if (iteration % 2 == 0) {
      worker.shutdown();
      worker.shutdown();
    }
  }
}

TEST(WorkerTest, shutdownAtIdleWaitTransition) {
  for (int iteration = 0; iteration < 64; ++iteration) {
    SCOPED_TRACE(iteration);
    bool firstCheck = true;
    std::promise<void> checking;
    auto checked = checking.get_future();
    vvw_gen::Worker worker([](auto &) {}, [&]() {
      if (firstCheck) {
        firstCheck = false;
        checking.set_value();
      }
      return false;
    });
    ASSERT_EQ(checked.wait_for(5s), std::future_status::ready);
    worker.shutdown();
  }
}

TEST(WorkerTest, shutdownWithBlockedPendingWork) {
  for (int iteration = 0; iteration < 64; ++iteration) {
    SCOPED_TRACE(iteration);
    bool pending = false;
    int processed = 0;
    vvw_gen::Worker worker(
        [&](auto &) {
          pending = false;
          ++processed;
        },
        [&]() { return pending; });
    worker.setBlock(true);
    {
      std::lock_guard<std::mutex> lock(worker.getMutex());
      pending = true;
    }
    worker.notifyWorkWasAdded();
    worker.shutdown();
    EXPECT_EQ(processed, 0);
  }
}

TEST(WorkerTest, shutdownWaitsForActiveCallback) {
  bool pending = true;
  bool processed = false;
  std::promise<void> entered;
  auto started = entered.get_future();
  std::promise<void> release;
  auto released = release.get_future();
  vvw_gen::Worker worker(
      [&](auto &lock) {
        lock.unlock();
        entered.set_value();
        released.wait();
        lock.lock();
        pending = false;
        processed = true;
      },
      [&]() { return pending; });
  const auto status = started.wait_for(5s);
  if (status != std::future_status::ready) {
    release.set_value();
    FAIL() << "Worker did not enter the callback";
  }

  auto stopped = std::async(std::launch::async, [&]() { worker.shutdown(); });
  EXPECT_EQ(stopped.wait_for(50ms), std::future_status::timeout);
  // Release before any fatal assertion or future destruction can join a thread.
  release.set_value();
  ASSERT_EQ(stopped.wait_for(5s), std::future_status::ready);
  stopped.get();
  EXPECT_TRUE(processed);
}
