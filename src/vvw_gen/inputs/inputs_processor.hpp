#ifndef VVW_INPUTS_PROCESSOR_HPP
#define VVW_INPUTS_PROCESSOR_HPP

#include <map>
#include <set>
#include <vvw_gen/concepts.hpp>

#include "raw_inputs.hpp"

BEGIN_VVW_GEN_LIB_NS

template <EnumType T>
class InputsProcessor;

template <EnumType T>
struct Inputs {
  CursorData cursorData;
  ScrollData scrollData;
  bool operator()(T input) const { return activeInputs_.contains(input); }

 private:
  std::set<T> activeInputs_{};

  void setActive(T input) { activeInputs_.insert(input); }

  friend class InputsProcessor<T>;
};

template <EnumType T>
class InputsProcessor {
 public:
  InputsProcessor(std::map<T, InputDefinition> inputDefs) {
    for (auto [input, def] : inputDefs) {
      keyCodeToInput_.emplace(def.keyCode, input);
      if (def.type == InputType::TOGGLE) {
        toggleKeys_.insert(input);
        toggleKeyShouldToggle_.emplace(input, true);
      }
    }
  }

  Inputs<T> operator()(RawInputs rawInputs) {
    Inputs<T> inputs{.cursorData = rawInputs.cursorData,
                     .scrollData = rawInputs.scrollData};
    for (auto &[keyCode, input] : keyCodeToInput_) {
      if (rawInputs.pressedKeyCodes.contains(keyCode)) {
        processPressedKey(inputs, input, keyCode);
      }
    }
    std::erase_if(toggleKeyCodesToCheck_, [&](int keyCode) {
      if (!rawInputs.pressedKeyCodes.contains(keyCode)) {
        T input = keyCodeToInput_[keyCode];
        toggleKeyShouldToggle_[input] = true;
        return true;
      }
      return false;
    });
    return inputs;
  }

 private:
  std::map<int, T> keyCodeToInput_{};

  std::set<T> toggleKeys_{};
  // The toggle keys we need to check to know if we should listen again or not
  std::set<int> toggleKeyCodesToCheck_{};
  std::map<T, bool> toggleKeyShouldToggle_{};

  void processPressedKey(Inputs<T> &inputs, T input, int keyCode) {
    // Continuous first
    if (!toggleKeys_.contains(input)) {
      inputs.setActive(input);
      return;
    }

    // Toggle check then
    if (!toggleKeyShouldToggle_[input]) {
      return;
    }

    // Here we need to toggle and deactivate toggling
    inputs.setActive(input);
    toggleKeyCodesToCheck_.insert(keyCode);
    toggleKeyShouldToggle_[input] = false;
  }
};

END_VVW_GEN_LIB_NS

#endif
