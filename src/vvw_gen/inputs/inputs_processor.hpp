#ifndef VVW_INPUTS_PROCESSOR_HPP
#define VVW_INPUTS_PROCESSOR_HPP
#include <map>
#include <optional>
#include <set>
#include <vector>
#include <vvw_gen/concepts.hpp>

#include "raw_inputs.hpp"

BEGIN_VVW_GEN_LIB_NS

template <EnumType T> class InputsProcessor;

template <EnumType T> struct Inputs {
  CursorData cursorData;
  ScrollData scrollData;

  bool operator()(T input, InputType type) const {
    if (type == InputType::TOGGLE) {
      return activeToggleInputs_.contains(input);
    } else {
      return activeContinuousInputs_.contains(input);
    }
  }

private:
  std::set<T> activeToggleInputs_{};
  std::set<T> activeContinuousInputs_{};

  void setActiveToggle(T input) { activeToggleInputs_.insert(input); }
  void setActiveContinuous(T input) { activeContinuousInputs_.insert(input); }

  friend class InputsProcessor<T>;
};

template <EnumType T> class InputsProcessor {
public:
  InputsProcessor(std::map<T, InputDefinition> inputDefs) {
    for (auto [input, def] : inputDefs) {
      keyCodeToInput_.emplace(def.keyCode, input);

      for (InputType type : def.types) {
        if (type == InputType::TOGGLE) {
          toggleKeys_.insert(input);
          toggleKeyShouldToggle_.emplace(input, true);
        } else if (type == InputType::CONTINUOUS) {
          continuousKeys_.insert(input);
        }
      }
    }
  }

  Inputs<T> operator()(RawInputs rawInputs) {
    Inputs<T> inputs;
    lastPressedKeyboardKeyCode_ = rawInputs.lastPressedKeyboardKeyCode;
    inputs.cursorData = rawInputs.cursorData;
    inputs.scrollData = rawInputs.scrollData;

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

  std::optional<int> getLastPressedKeyboardKeyCode() const {
    return lastPressedKeyboardKeyCode_;
  }

private:
  std::map<int, T> keyCodeToInput_{};
  std::set<T> toggleKeys_{};
  std::set<T> continuousKeys_{};
  std::set<int> toggleKeyCodesToCheck_{};
  std::map<T, bool> toggleKeyShouldToggle_{};
  std::optional<int> lastPressedKeyboardKeyCode_{};

  void processPressedKey(Inputs<T> &inputs, T input, int keyCode) {
    // Continuous - seulement si configuré
    if (continuousKeys_.contains(input)) {
      inputs.setActiveContinuous(input);
    }

    // Toggle - seulement si configuré
    if (toggleKeys_.contains(input) && toggleKeyShouldToggle_[input]) {
      inputs.setActiveToggle(input);
      toggleKeyCodesToCheck_.insert(keyCode);
      toggleKeyShouldToggle_[input] = false;
    }
  }
};

END_VVW_GEN_LIB_NS
#endif
