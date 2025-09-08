#include <gtest/gtest.h>

#include <vvw_gen/vvw_gen.hpp>

TEST(WorkerTest, usage) {
  int value = 1;
  std::function<void(std::unique_lock<std::mutex> &)> increaseValue(
      [&value](auto &lock) { value += 1; });
  std::function<bool()> hasWorkToDo(
      [&value]() { return value > 1 && value < 10; });
  vvw_gen::Worker worker(increaseValue, hasWorkToDo);
  EXPECT_EQ(value, 1);
  value += 1;
  EXPECT_EQ(value, 2);
  worker.notifyWorkWasAdded();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  EXPECT_EQ(value, 10);
}

TEST(WorkerTest, block) {
  int value = 1;
  std::function<void(std::unique_lock<std::mutex> &)> increaseValue(
      [&value](auto &lock) { value += 1; });
  std::function<bool()> hasWorkToDo(
      [&value]() { return value > 1 && value < 10; });
  std::mutex mtx;
  vvw_gen::Worker worker(increaseValue, hasWorkToDo);
  EXPECT_EQ(value, 1);
  value += 1;
  EXPECT_EQ(value, 2);
  worker.setBlock(true);
  worker.notifyWorkWasAdded();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  EXPECT_EQ(value, 2);
}
