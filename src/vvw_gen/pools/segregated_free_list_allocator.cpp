#include "segregated_free_list_allocator.hpp"

#include <cassert>
BEGIN_VVW_GEN_LIB_NS
SegregatedFreeListAllocator::SegregatedFreeListAllocator(uint32_t poolSize)
    : poolSize_(poolSize) {}
std::optional<uint32_t> SegregatedFreeListAllocator::allocate(
    uint32_t nConsecutiveAllocations) {
  if (nConsecsAllocToIndex_.contains(nConsecutiveAllocations)) {
    return removeFreeAlloc_(nConsecutiveAllocations);
  }
  uint32_t outputIndex = head_;
  head_ += nConsecutiveAllocations;
  if (outputIndex >= poolSize_) {
    return std::nullopt;
  }
  return outputIndex;
}
void SegregatedFreeListAllocator::deallocate(uint32_t index,
                                             uint32_t nConsecutiveAllocations) {
  addFreeAlloc_(nConsecutiveAllocations, index);
}

void SegregatedFreeListAllocator::addFreeAlloc_(
    uint32_t nConsecutiveAllocations, uint32_t index) {
  if (!nConsecsAllocToIndex_.contains(nConsecutiveAllocations)) {
    nConsecsAllocToIndex_[nConsecutiveAllocations] = std::queue<uint32_t>();
  }
  auto &allocQueue = nConsecsAllocToIndex_[nConsecutiveAllocations];
  allocQueue.push(index);
}

uint32_t SegregatedFreeListAllocator::removeFreeAlloc_(
    uint32_t nConsecutiveAllocations) {
  assert(nConsecsAllocToIndex_.contains(nConsecutiveAllocations));
  auto &allocQueue = nConsecsAllocToIndex_[nConsecutiveAllocations];
  uint32_t index = allocQueue.front();
  allocQueue.pop();
  if (allocQueue.empty()) {
    nConsecsAllocToIndex_.erase(nConsecutiveAllocations);
  }
  return index;
}
END_VVW_GEN_LIB_NS
