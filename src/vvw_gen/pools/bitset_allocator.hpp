#ifndef VVW_BUCKET_BITSET_ALLOCATOR_HPP
#define VVW_BUCKET_BITSET_ALLOCATOR_HPP

#include <cstdint>
#include <optional>
#include <vector>

#include "vvw_gen/macros.hpp"

BEGIN_VVW_GEN_LIB_NS

class BitSetAllocator {
 public:
  BitSetAllocator(uint32_t poolSize);

  std::optional<uint32_t> allocate(uint32_t index,
                                   std::optional<uint32_t> nBucketsCheckedMax);
  void deallocate(uint32_t index);
  bool isAllocated(uint32_t index);

 private:
  uint32_t poolSize_;
  std::vector<uint32_t> pool_;

  bool isBucketFull_(uint32_t bucketIndex);
  uint32_t getFirstFreeBitPos_(uint32_t bucketIndex);
};

END_VVW_GEN_LIB_NS

#endif
