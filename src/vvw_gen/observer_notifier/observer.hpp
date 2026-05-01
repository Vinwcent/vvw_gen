#ifndef VVW_GEN_OBSERVER_NOTIFIER_OBSERVER_HPP
#define VVW_GEN_OBSERVER_NOTIFIER_OBSERVER_HPP

#include <concepts>

#include "vvw_gen/macros.hpp"
#include "vvw_gen/observer_notifier/observer_notifier_payload_trait.hpp"

BEGIN_VVW_GEN_LIB_NS

template <typename Derived, typename Payload>
concept HasOn = requires(Derived &derived, const Payload &payload) {
  { derived.on(payload) } -> std::same_as<void>;
};

template <auto Event>
class ObserverFor {
public:
  using Payload = ObserverNotifierPayload<Event>;

  virtual ~ObserverFor() = default;

  virtual void notify(const Payload &payload) = 0;
};

template <typename Derived, auto Event>
class ObserverEvent : public ObserverFor<Event> {
public:
  using Payload = ObserverNotifierPayload<Event>;

  ObserverEvent() = default;
  ~ObserverEvent() override = default;

  void notify(const Payload &payload) override {
    static_assert(HasOn<Derived, Payload>,
                  "Observer must implement on(const Payload&) for this event");
    static_cast<Derived &>(*this).on(payload);
  }
};

template <typename Derived, auto... Events>
class Observer : public ObserverEvent<Derived, Events>... {
public:
  static_assert(sizeof...(Events) > 0,
                "Observer must subscribe to at least one event type");

  Observer() = default;
  ~Observer() override = default;

  Observer(const Observer &) = delete;
  Observer &operator=(const Observer &) = delete;

  Observer(Observer &&other) noexcept = default;
  Observer &operator=(Observer &&other) noexcept = default;
};

END_VVW_GEN_LIB_NS

#endif
