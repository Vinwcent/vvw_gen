#ifndef VVW_GEN_OBSERVER_NOTIFIER_PAYLOAD_TRAIT_HPP
#define VVW_GEN_OBSERVER_NOTIFIER_PAYLOAD_TRAIT_HPP

#include <type_traits>

#include "vvw_gen/macros.hpp"

BEGIN_VVW_GEN_LIB_NS

template <class>
inline constexpr bool always_false_v = false;

template <auto Event>
struct ObserverNotifierPayloadTrait {
  static_assert(
      always_false_v<std::integral_constant<decltype(Event), Event>>,
      "ObserverNotifierPayloadTrait<Event> must be specialized for this event");
};

template <auto Event>
using ObserverNotifierPayload = typename ObserverNotifierPayloadTrait<Event>::type;

END_VVW_GEN_LIB_NS

#endif
