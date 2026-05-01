#ifndef VVW_GEN_OBSERVER_NOTIFIER_ENUM_CONCEPTS_HPP
#define VVW_GEN_OBSERVER_NOTIFIER_ENUM_CONCEPTS_HPP

#include <type_traits>

#include "vvw_gen/macros.hpp"

BEGIN_VVW_GEN_LIB_NS

template <typename T>
concept Enum = std::is_enum_v<T>;

END_VVW_GEN_LIB_NS

#endif
