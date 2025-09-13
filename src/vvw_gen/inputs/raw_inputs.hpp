#ifndef VVW_RAW_INPUTS_HPP
#define VVW_RAW_INPUTS_HPP

#include <set>

#include "vvw_gen/macros.hpp"

BEGIN_VVW_GEN_LIB_NS

struct CursorData {
  double x;
  double y;
  double deltaX;
  double deltaY;
};

struct ScrollData {
  double offsetX;
  double offsetY;
};

struct RawInputs {
  std::set<int> pressedKeyCodes;
  CursorData cursorData;
  ScrollData scrollData;
};

enum class InputType { CONTINUOUS, TOGGLE };

struct InputDefinition {
  int keyCode;
  InputType type;
};

END_VVW_GEN_LIB_NS

#endif
