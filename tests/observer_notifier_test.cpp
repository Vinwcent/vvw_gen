#include <gtest/gtest.h>

#include "vvw_gen/observer_notifier/notifier.hpp"

namespace {

enum class TestEvent { Count, Label, Ignored };

struct CountPayload {
  int value = 0;
};

struct LabelPayload {
  const char *value = nullptr;
};

struct IgnoredPayload {
  bool value = false;
};

} // namespace

template <>
struct vvw_gen::ObserverNotifierPayloadTrait<TestEvent::Count> {
  using type = CountPayload;
};

template <>
struct vvw_gen::ObserverNotifierPayloadTrait<TestEvent::Label> {
  using type = LabelPayload;
};

template <>
struct vvw_gen::ObserverNotifierPayloadTrait<TestEvent::Ignored> {
  using type = IgnoredPayload;
};

namespace {

using TestNotifier =
    vvw_gen::Notifier<TestEvent, TestEvent::Count, TestEvent::Label,
                      TestEvent::Ignored>;

class CountingObserver
    : public vvw_gen::Observer<CountingObserver, TestEvent::Count,
                               TestEvent::Label> {
public:
  void on(const CountPayload &payload) { count += payload.value; }
  void on(const LabelPayload &payload) { label = payload.value; }

  int count = 0;
  const char *label = nullptr;
};

class CountOnlyObserver
    : public vvw_gen::Observer<CountOnlyObserver, TestEvent::Count> {
public:
  void on(const CountPayload &payload) { count += payload.value; }

  int count = 0;
};

static_assert(std::is_base_of_v<vvw_gen::ObserverFor<TestEvent::Count>,
                                CountingObserver>);
static_assert(!std::is_base_of_v<vvw_gen::ObserverFor<TestEvent::Ignored>,
                                 CountingObserver>);

TEST(ObserverNotifier, NotifyCallsSubscribedEventObserver) {
  TestNotifier notifier;
  CountingObserver observer;

  notifier.subscribe<TestEvent::Count>(observer);
  notifier.notify<TestEvent::Count>(CountPayload{.value = 3});

  EXPECT_EQ(observer.count, 3);
}

TEST(ObserverNotifier, VariadicSubscribeHandlesMultipleEvents) {
  TestNotifier notifier;
  CountingObserver observer;

  notifier.subscribe<TestEvent::Count, TestEvent::Label>(observer);
  notifier.notify<TestEvent::Count>(CountPayload{.value = 7});
  notifier.notify<TestEvent::Label>(LabelPayload{.value = "hello"});

  EXPECT_EQ(observer.count, 7);
  EXPECT_STREQ(observer.label, "hello");
}

TEST(ObserverNotifier, NotifyOnlyTargetsSubscribedEvent) {
  TestNotifier notifier;
  CountingObserver observer;

  notifier.subscribe<TestEvent::Label>(observer);
  notifier.notify<TestEvent::Count>(CountPayload{.value = 11});
  notifier.notify<TestEvent::Label>(LabelPayload{.value = "label"});

  EXPECT_EQ(observer.count, 0);
  EXPECT_STREQ(observer.label, "label");
}

TEST(ObserverNotifier, MultipleObserversReceiveSameEvent) {
  TestNotifier notifier;
  CountingObserver observerA;
  CountOnlyObserver observerB;

  notifier.subscribe<TestEvent::Count>(observerA);
  notifier.subscribe<TestEvent::Count>(observerB);
  notifier.notify<TestEvent::Count>(CountPayload{.value = 5});

  EXPECT_EQ(observerA.count, 5);
  EXPECT_EQ(observerB.count, 5);
}

TEST(ObserverNotifier, DuplicateSubscribeDoesNotDuplicateNotification) {
  TestNotifier notifier;
  CountingObserver observer;

  notifier.subscribe<TestEvent::Count>(observer);
  notifier.subscribe<TestEvent::Count>(observer);
  notifier.notify<TestEvent::Count>(CountPayload{.value = 4});

  EXPECT_EQ(observer.count, 4);
}

TEST(ObserverNotifier, VariadicUnsubscribeRemovesSelectedEvents) {
  TestNotifier notifier;
  CountingObserver observer;

  notifier.subscribe<TestEvent::Count, TestEvent::Label>(observer);
  notifier.unsubscribe<TestEvent::Count>(observer);
  notifier.notify<TestEvent::Count>(CountPayload{.value = 9});
  notifier.notify<TestEvent::Label>(LabelPayload{.value = "still subscribed"});

  EXPECT_EQ(observer.count, 0);
  EXPECT_STREQ(observer.label, "still subscribed");
}

TEST(ObserverNotifier, ClearRemovesAllObservers) {
  TestNotifier notifier;
  CountingObserver observer;

  notifier.subscribe<TestEvent::Count, TestEvent::Label>(observer);
  notifier.clear();
  notifier.notify<TestEvent::Count>(CountPayload{.value = 3});
  notifier.notify<TestEvent::Label>(LabelPayload{.value = "ignored"});

  EXPECT_EQ(observer.count, 0);
  EXPECT_EQ(observer.label, nullptr);
}

} // namespace
