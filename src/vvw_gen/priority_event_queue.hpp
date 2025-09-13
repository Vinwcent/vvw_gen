#ifndef VVW_PRIORITY_EVENT_QUEUE_HPP
#define VVW_PRIORITY_EVENT_QUEUE_HPP

#include <algorithm>
#include <map>
#include <mutex>
#include <set>

#include "concepts.hpp"
#include "vvw_gen/macros.hpp"
#include "vvw_gen/type_erasure/args_storage.hpp"
#include "vvw_gen/type_erasure/function_wrappers.hpp"
#include "vvw_gen/worker.hpp"

BEGIN_VVW_GEN_LIB_NS

template <EnumType Event>
struct PriorityEventQueueConfig {
  std::map<Event, int> eventPriorities{};
  std::map<Event, std::unique_ptr<FunctionWrapperTypeEraser>> eventsFunctions{};

  void clear() {
    eventPriorities.clear();
    eventsFunctions.clear();
  }
};

template <EnumType Event>
class PriorityEventQueue {
 public:
  PriorityEventQueue(PriorityEventQueueConfig<Event>&& config)
      : worker(
            [this](std::unique_lock<std::mutex>& lock) {
              this->processNextEvent_(lock);
            },
            [this]() { return this->hasEventsToProcess_(); })

  {
    checkValidity_(config);
    initOrderedEvents_(config.eventPriorities);
    eventsFunctions_ = std::move(config.eventsFunctions);
    for (auto& [event, _] : config.eventPriorities) {
      eventToArgs_[event].clear();
    }
  }

  template <typename... Args>
  void addEvent(Event event, Args... args) {
    std::lock_guard<std::mutex> lock(worker.getMutex());
    eventToArgs_[event].push_back(
        std::make_unique<ArgsStorage<Args...>>(args...));
    worker.notifyWorkWasAdded();
  }

  template <typename... Args>
  void alterStoredEvent(Event event,
                        std::function<void(Args*...)> alterFunction) {
    std::lock_guard<std::mutex> lock(worker.getMutex());
    for (auto& args : eventToArgs_[event]) {
      args->applyFunction(alterFunction);
    }
  }

  int getNPendingEvents(Event event) {
    std::lock_guard<std::mutex> lock(worker.getMutex());
    return eventToArgs_[event].size();
  }

  void setEventProcessing(bool process) { worker.setBlock(!process); }

  bool isProcessingAnEvent() { return worker.isWorking(); }

 private:
  std::vector<Event> orderedEvents_{};
  std::map<Event, std::unique_ptr<FunctionWrapperTypeEraser>>
      eventsFunctions_{};
  std::map<Event, std::vector<std::unique_ptr<ArgsStorageTypeEraser>>>
      eventToArgs_{};

  Worker worker;

  void processNextEvent_(std::unique_lock<std::mutex>& lock) {
    for (auto event : orderedEvents_) {
      int nArgsToProcess = eventToArgs_[event].size();
      if (nArgsToProcess < 1) {
        continue;
      }
      std::unique_ptr<ArgsStorageTypeEraser> argsStorage =
          std::move(eventToArgs_[event][0]);
      eventToArgs_[event].erase(eventToArgs_[event].begin());
      lock.unlock();
      std::vector<void*> args = argsStorage->getArgPtrs();
      (*eventsFunctions_[event])(args);
      lock.lock();
      break;
    }
  }

  bool hasEventsToProcess_() {
    for (auto event : orderedEvents_) {
      int nArgsToProcess = eventToArgs_[event].size();
      if (nArgsToProcess > 0) {
        return true;
      }
    }
    return false;
  }

  void checkValidity_(const PriorityEventQueueConfig<Event>& config) {
    std::set<Event> events;
    std::set<int> priorities;

    for (const auto& [event, priority] : config.eventPriorities) {
      if (!events.insert(event).second) {
        throw std::invalid_argument("Duplicate event detected");
      }
      if (!priorities.insert(priority).second) {
        throw std::invalid_argument("Duplicate priority detected");
      }
    }
  }

  void initOrderedEvents_(std::map<Event, int>& eventPriorities) {
    std::vector<std::pair<Event, int>> sorted_pairs(eventPriorities.begin(),
                                                    eventPriorities.end());
    std::sort(sorted_pairs.begin(), sorted_pairs.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    orderedEvents_.reserve(sorted_pairs.size());
    for (const auto& [event, _] : sorted_pairs) {
      orderedEvents_.push_back(event);
    }
  }
};

END_VVW_GEN_LIB_NS
#endif
