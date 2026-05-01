#ifndef VVW_GEN_OBSERVER_NOTIFIER_NOTIFIER_HPP
#define VVW_GEN_OBSERVER_NOTIFIER_NOTIFIER_HPP

#include <algorithm>
#include <tuple>
#include <type_traits>
#include <vector>

#include "vvw_gen/macros.hpp"
#include "vvw_gen/observer_notifier/enum_concepts.hpp"
#include "vvw_gen/observer_notifier/observer.hpp"

BEGIN_VVW_GEN_LIB_NS

template <auto Event>
struct EventObserverList {
  std::vector<ObserverFor<Event> *> observers;
};

template <Enum EventEnum, EventEnum... Events>
class Notifier {
public:
  static_assert(sizeof...(Events) > 0,
                "Notifier must declare at least one event");

  Notifier() = default;
  ~Notifier() = default;

  Notifier(const Notifier &) = delete;
  Notifier &operator=(const Notifier &) = delete;

  Notifier(Notifier &&other) noexcept = default;
  Notifier &operator=(Notifier &&other) noexcept = default;

  template <EventEnum... SubscribedEvents, typename ObserverT>
  void subscribe(ObserverT &observer) {
    (subscribeOne_<SubscribedEvents>(observer), ...);
  }

  template <EventEnum... UnsubscribedEvents, typename ObserverT>
  void unsubscribe(ObserverT &observer) {
    (unsubscribeOne_<UnsubscribedEvents>(observer), ...);
  }

  void clear() { observersByEvent_ = {}; }

  template <EventEnum Event>
  void notify(const ObserverNotifierPayload<Event> &payload) {
    static_assert(containsEvent_<Event>(),
                  "Notifier does not declare this event");
    for (auto *observer : getObserverList_<Event>().observers) {
      observer->notify(payload);
    }
  }

private:
  std::tuple<EventObserverList<Events>...> observersByEvent_;

  template <EventEnum Event>
  static consteval bool containsEvent_() {
    return ((Events == Event) || ...);
  }

  template <EventEnum Event, typename ObserverT>
  void subscribeOne_(ObserverT &observer) {
    static_assert(containsEvent_<Event>(),
                  "Notifier does not declare this event");
    static_assert(std::is_base_of_v<ObserverFor<Event>, ObserverT>,
                  "Observer must inherit ObserverFor<Event>");
    auto &observers = getObserverList_<Event>().observers;
    auto *typedObserver = static_cast<ObserverFor<Event> *>(&observer);
    if (std::find(observers.begin(), observers.end(), typedObserver) ==
        observers.end()) {
      observers.push_back(typedObserver);
    }
  }

  template <EventEnum Event, typename ObserverT>
  void unsubscribeOne_(ObserverT &observer) {
    static_assert(containsEvent_<Event>(),
                  "Notifier does not declare this event");
    static_assert(std::is_base_of_v<ObserverFor<Event>, ObserverT>,
                  "Observer must inherit ObserverFor<Event>");
    auto &observers = getObserverList_<Event>().observers;
    auto *typedObserver = static_cast<ObserverFor<Event> *>(&observer);
    observers.erase(std::remove(observers.begin(), observers.end(), typedObserver),
                    observers.end());
  }

  template <EventEnum Event>
  EventObserverList<Event> &getObserverList_() {
    return std::get<EventObserverList<Event>>(observersByEvent_);
  }
};

END_VVW_GEN_LIB_NS

#endif
