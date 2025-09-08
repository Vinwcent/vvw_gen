#ifndef VVW_FUNCTION_WRAPPERS_HPP
#define VVW_FUNCTION_WRAPPERS_HPP

#include <functional>

#include "vvw_gen/macros.hpp"

BEGIN_VVW_GEN_LIB_NS

struct FunctionWrapperTypeEraser {
  virtual ~FunctionWrapperTypeEraser() = default;
  virtual void operator()(const std::vector<void *> &args) = 0;
};

template <typename... Args>
struct FunctionWrapper : FunctionWrapperTypeEraser {
 public:
  explicit FunctionWrapper(std::function<void(Args...)> funcToWrap)
      : wrappedFunc_(funcToWrap) {}

  void operator()(const std::vector<void *> &args) override {
    callImplem(args, std::index_sequence_for<Args...>{});
  }

 private:
  std::function<void(Args...)> wrappedFunc_;

  template <size_t... Is>
  void callImplem(const std::vector<void *> &args, std::index_sequence<Is...>) {
    wrappedFunc_(*static_cast<typename std::remove_reference<Args>::type *>(
        args[Is])...);
  }
};

END_VVW_GEN_LIB_NS

#endif
