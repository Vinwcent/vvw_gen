#ifndef VVW_SEGREGATED_HANDLE_POOL_HPP
#define VVW_SEGREGATED_HANDLE_POOL_HPP
#include <cassert>
#include <unordered_map>

#include "segregated_free_list_allocator.hpp"
BEGIN_VVW_GEN_LIB_NS
template <typename T, typename HashingFunctor = std::hash<T>>
class SegregatedHandlePool {
 public:
  SegregatedHandlePool(uint32_t poolSize) : allocator_(poolSize) {}
  std::optional<uint32_t> allocate(T allocationKey,
                                   uint32_t nConsecutiveAllocations) {
    auto allocIndex = allocator_.allocate(nConsecutiveAllocations);
    if (!allocIndex.has_value()) {
      return std::nullopt;
    }
    mapping_[allocationKey] = allocIndex.value();
    return allocIndex.value();
  }
  void deallocate(T allocationKey, uint32_t nConsecutiveAllocations) {
    assert(isAllocated(allocationKey));
    auto index = mapping_[allocationKey];
    allocator_.deallocate(index, nConsecutiveAllocations);
    mapping_.erase(allocationKey);
  }
  bool isAllocated(T allocationKey) { return mapping_.contains(allocationKey); }
  uint32_t get(T allocationKey) {
    assert(isAllocated(allocationKey));
    return mapping_[allocationKey];
  }

 private:
  SegregatedFreeListAllocator allocator_;
  std::unordered_map<T, uint32_t, HashingFunctor> mapping_;
};
END_VVW_GEN_LIB_NS
#endif
