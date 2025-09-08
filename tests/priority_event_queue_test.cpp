#include <gtest/gtest.h>

#include <vvw_gen/vvw_gen.hpp>

enum class DummyEvent { EVENT_1 };

TEST(PriorityEventQueueTest, construction) {
  int value = 1;
  std::function<void(int)> valueAdd(
      [&value](int increment) { value += increment; });
  vvw_gen_lib::FunctionWrapper<int> wrapper(valueAdd);

  std::map<DummyEvent, std::unique_ptr<vvw_gen_lib::FunctionWrapperTypeEraser>>
      erasedTypeWrapper;
  erasedTypeWrapper[DummyEvent::EVENT_1] =
      std::make_unique<vvw_gen_lib::FunctionWrapper<int>>(valueAdd);

  vvw_gen_lib::PriorityEventQueueConfig<DummyEvent> config;
  config.eventPriorities = {{DummyEvent::EVENT_1, 0}};
  config.eventsFunctions = std::move(erasedTypeWrapper);

  EXPECT_NO_THROW({
    auto queue = vvw_gen_lib::PriorityEventQueue<DummyEvent>(std::move(config));
  });
}

TEST(PriorityEventQueueTest, addEventTest) {
  int value = 1;
  std::function<void(int, int)> valueAffine(
      [&value](int a, int b) { value = value * a + b; });

  std::map<DummyEvent, std::unique_ptr<vvw_gen_lib::FunctionWrapperTypeEraser>>
      erasedTypeWrapper;
  erasedTypeWrapper[DummyEvent::EVENT_1] =
      std::make_unique<vvw_gen_lib::FunctionWrapper<int, int>>(valueAffine);

  vvw_gen_lib::PriorityEventQueueConfig<DummyEvent> config;
  config.eventPriorities = {{DummyEvent::EVENT_1, 0}};
  config.eventsFunctions = std::move(erasedTypeWrapper);

  auto queue = vvw_gen_lib::PriorityEventQueue<DummyEvent>(std::move(config));

  queue.addEvent(DummyEvent::EVENT_1, 4, 1);
  queue.addEvent(DummyEvent::EVENT_1, 6, 1);
  queue.addEvent(DummyEvent::EVENT_1, 8, 1);

  EXPECT_EQ(queue.getNPendingEvents(DummyEvent::EVENT_1), 3);
}

TEST(PriorityEventQueueTest, processEvent) {
  int value = 1;
  std::function<void(int, int)> valueAffine(
      [&value](int a, int b) { value = value * a + b; });

  std::map<DummyEvent, std::unique_ptr<vvw_gen_lib::FunctionWrapperTypeEraser>>
      erasedTypeWrapper;
  erasedTypeWrapper[DummyEvent::EVENT_1] =
      std::make_unique<vvw_gen_lib::FunctionWrapper<int, int>>(valueAffine);

  vvw_gen_lib::PriorityEventQueueConfig<DummyEvent> config;
  config.eventPriorities = {{DummyEvent::EVENT_1, 0}};
  config.eventsFunctions = std::move(erasedTypeWrapper);

  auto queue = vvw_gen_lib::PriorityEventQueue<DummyEvent>(std::move(config));
  queue.addEvent(DummyEvent::EVENT_1, 2, 3);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  EXPECT_EQ(value, 5);
  queue.addEvent(DummyEvent::EVENT_1, 3, 3);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  EXPECT_EQ(value, 18);
}

TEST(PriorityEventQueueTest, addEventAlterAndProcess) {
  int value = 1;
  std::function<void(int, int)> valueAffine(
      [&value](int a, int b) { value = value * a + b; });

  std::map<DummyEvent, std::unique_ptr<vvw_gen_lib::FunctionWrapperTypeEraser>>
      erasedTypeWrapper;
  erasedTypeWrapper[DummyEvent::EVENT_1] =
      std::make_unique<vvw_gen_lib::FunctionWrapper<int, int>>(valueAffine);

  vvw_gen_lib::PriorityEventQueueConfig<DummyEvent> config;
  config.eventPriorities = {{DummyEvent::EVENT_1, 0}};
  config.eventsFunctions = std::move(erasedTypeWrapper);

  auto queue = vvw_gen_lib::PriorityEventQueue<DummyEvent>(std::move(config));
  queue.setEventProcessing(false);
  queue.addEvent(DummyEvent::EVENT_1, 2, 3);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  EXPECT_EQ(value, 1);
  queue.alterStoredEvent(DummyEvent::EVENT_1,
                         std::function<void(int*, int*)>([](int* a, int* b) {
                           *a += 1;
                           *b += 1;
                         }));
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  EXPECT_EQ(value, 1);
  queue.setEventProcessing(true);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  EXPECT_EQ(value, 7);
}

TEST(PriorityEventQueueTest, processEventWhileAdding) {
  int value = 1;
  std::function<void(int, int)> valueAffine(
      [&value](int a, int b) { value = value * a + b; });

  std::map<DummyEvent, std::unique_ptr<vvw_gen_lib::FunctionWrapperTypeEraser>>
      erasedTypeWrapper;
  erasedTypeWrapper[DummyEvent::EVENT_1] =
      std::make_unique<vvw_gen_lib::FunctionWrapper<int, int>>(valueAffine);

  vvw_gen_lib::PriorityEventQueueConfig<DummyEvent> config;
  config.eventPriorities = {{DummyEvent::EVENT_1, 0}};
  config.eventsFunctions = std::move(erasedTypeWrapper);

  auto queue = vvw_gen_lib::PriorityEventQueue<DummyEvent>(std::move(config));
  queue.addEvent(DummyEvent::EVENT_1, 2, 3);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  EXPECT_EQ(value, 5);
  for (int i = 0; i < 100; ++i) {
    queue.addEvent(DummyEvent::EVENT_1, 1, 1);
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  EXPECT_EQ(value, 105);
}

TEST(PriorityEventQueueTest, addManyEventsLaunchAndAlter) {
  int value = 1;
  std::function<void(int)> valueAffine([&value](int a) { value = value + a; });

  std::map<DummyEvent, std::unique_ptr<vvw_gen_lib::FunctionWrapperTypeEraser>>
      erasedTypeWrapper;
  erasedTypeWrapper[DummyEvent::EVENT_1] =
      std::make_unique<vvw_gen_lib::FunctionWrapper<int>>(valueAffine);

  vvw_gen_lib::PriorityEventQueueConfig<DummyEvent> config;
  config.eventPriorities = {{DummyEvent::EVENT_1, 0}};
  config.eventsFunctions = std::move(erasedTypeWrapper);

  auto queue = vvw_gen_lib::PriorityEventQueue<DummyEvent>(std::move(config));
  queue.setEventProcessing(false);
  for (int i = 0; i < 10; ++i) {
    queue.addEvent(DummyEvent::EVENT_1, 1);
  }
  queue.alterStoredEvent(DummyEvent::EVENT_1,
                         std::function<void(int*)>([](int* a) { *a += 1; }));
  for (int i = 0; i < 80; ++i) {
    queue.addEvent(DummyEvent::EVENT_1, 1);
  }
  queue.setEventProcessing(true);
  std::this_thread::sleep_for(std::chrono::milliseconds(300));
  EXPECT_EQ(value, 101);

  EXPECT_NO_THROW({
    for (int i = 0; i < 10; ++i) {
      for (int j = 0; j < 1000; ++j) {
        queue.addEvent(DummyEvent::EVENT_1, 1);
      }
      queue.alterStoredEvent(DummyEvent::EVENT_1, std::function<void(int*)>(
                                                      [](int* a) { *a += 1; }));
    }
  });
}

TEST(PriorityEventQueueTest, alterWhileProcessing) {
  int value = 1;
  std::function<void(int)> valueAffine([&value](int a) {
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    value = value + a;
  });

  std::map<DummyEvent, std::unique_ptr<vvw_gen_lib::FunctionWrapperTypeEraser>>
      erasedTypeWrapper;
  erasedTypeWrapper[DummyEvent::EVENT_1] =
      std::make_unique<vvw_gen_lib::FunctionWrapper<int>>(valueAffine);

  vvw_gen_lib::PriorityEventQueueConfig<DummyEvent> config;
  config.eventPriorities = {{DummyEvent::EVENT_1, 0}};
  config.eventsFunctions = std::move(erasedTypeWrapper);

  auto queue = vvw_gen_lib::PriorityEventQueue<DummyEvent>(std::move(config));
  queue.addEvent(DummyEvent::EVENT_1, 1);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  EXPECT_EQ(value, 1);
  std::this_thread::sleep_for(std::chrono::milliseconds(150));
  EXPECT_EQ(value, 2);
  queue.addEvent(DummyEvent::EVENT_1, 1);
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  queue.alterStoredEvent(DummyEvent::EVENT_1,
                         std::function<void(int*)>([](int* a) { *a += 1; }));
  std::this_thread::sleep_for(std::chrono::milliseconds(300));
  EXPECT_EQ(value, 3);
}

TEST(PriorityEventQueueTest, checkIsProcessing) {
  int value = 1;
  std::function<void(int)> valueAffine([&value](int a) {
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    value = value + a;
  });

  std::map<DummyEvent, std::unique_ptr<vvw_gen_lib::FunctionWrapperTypeEraser>>
      erasedTypeWrapper;
  erasedTypeWrapper[DummyEvent::EVENT_1] =
      std::make_unique<vvw_gen_lib::FunctionWrapper<int>>(valueAffine);

  vvw_gen_lib::PriorityEventQueueConfig<DummyEvent> config;
  config.eventPriorities = {{DummyEvent::EVENT_1, 0}};
  config.eventsFunctions = std::move(erasedTypeWrapper);

  auto queue = vvw_gen_lib::PriorityEventQueue<DummyEvent>(std::move(config));
  queue.addEvent(DummyEvent::EVENT_1, 1);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  EXPECT_TRUE(queue.isProcessingAnEvent());
  std::this_thread::sleep_for(std::chrono::milliseconds(150));
  EXPECT_FALSE(queue.isProcessingAnEvent());
}

TEST(PriorityEventQueueBuilderTest, builder) {
  int value = 1;
  std::function<void(int, int)> valueAffine(
      [&value](int a, int b) { value = value * a + b; });

  vvw_gen_lib::PriorityEventQueueBuilder<DummyEvent> builder{};
  builder.registerEvent(DummyEvent::EVENT_1, 1, valueAffine);

  auto queue = builder.build();
  queue->addEvent(DummyEvent::EVENT_1, 2, 3);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  EXPECT_EQ(value, 5);
}

