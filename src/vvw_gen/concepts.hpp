#ifndef VVW_CONCEPTS_HPP
#define VVW_CONCEPTS_HPP

#include <type_traits>

#include "vvw_gen/macros.hpp"

BEGIN_VVW_GEN_LIB_NS

template <typename T>
concept EnumType = std::is_enum_v<T> && !std::is_convertible_v<T, int>;

END_VVW_GEN_LIB_NS

#endif
