#ifndef VVW_SEGREGATED_FREE_LIST_ALLOCATOR_HPP
#define VVW_SEGREGATED_FREE_LIST_ALLOCATOR_HPP
#include <cstdint>
#include <map>
#include <optional>
#include <queue>

#include "vvw_gen/macros.hpp"
BEGIN_VVW_GEN_LIB_NS
class SegregatedFreeListAllocator {
 public:
  SegregatedFreeListAllocator(uint32_t poolSize);
  std::optional<uint32_t> allocate(uint32_t nConsecutiveAllocations);
  void deallocate(uint32_t index, uint32_t nConsecutiveAllocations);

 private:
  uint32_t head_ = 0;
  uint32_t poolSize_;
  std::map<uint32_t, std::queue<uint32_t>> nConsecsAllocToIndex_;
  void addFreeAlloc_(uint32_t nConsecutiveAllocations, uint32_t index);
  uint32_t removeFreeAlloc_(uint32_t nConsecutiveAllocations);
};
END_VVW_GEN_LIB_NS
#endif
