#include "bitset_allocator.hpp"

#include <cassert>
#include <stdexcept>

#if defined(_MSC_VER)
#include <intrin.h>
#endif

#include "vvw_gen/pools/bucket_allocation_info.hpp"

constexpr uint32_t N_BITS_IN_UINT = 32;
constexpr uint32_t FULL_BUCKET = 0xFFFFFFFF;

BEGIN_VVW_GEN_LIB_NS

BitSetAllocator::BitSetAllocator(uint32_t poolSize)
    : poolSize_(poolSize), pool_(poolSize / N_BITS_IN_UINT) {}

std::optional<uint32_t> BitSetAllocator::allocate(
    uint32_t index, std::optional<uint32_t> nBucketsCheckedMax) {
  uint32_t bucketIndex = index / N_BITS_IN_UINT;
  uint32_t nBucketChecked = 0;

  uint32_t maxBucketsToCheck = nBucketsCheckedMax.has_value()
                                   ? nBucketsCheckedMax.value()
                                   : pool_.size();
  while (isBucketFull_(bucketIndex)) {
    if (nBucketChecked >= maxBucketsToCheck) {
      return std::nullopt;
    }
    ++nBucketChecked;
    ++bucketIndex;
    if (bucketIndex >= pool_.size()) {
      bucketIndex = 0;
    }
  }
  auto offsetInBucket = getFirstFreeBitPos_(bucketIndex);
  pool_[bucketIndex] |= (1u << offsetInBucket);
  return bucketIndex * N_BITS_IN_UINT + offsetInBucket;
}

bool BitSetAllocator::isBucketFull_(uint32_t bucketIndex) {
  if (bucketIndex >= pool_.size()) {
    throw std::out_of_range(
        "Tried to check a pack at an index that doesn't exist");
  }
  return pool_[bucketIndex] == FULL_BUCKET;
}

void BitSetAllocator::deallocate(uint32_t index) {
  auto allocInfo = BucketAllocationInfo(index, N_BITS_IN_UINT);
  pool_[allocInfo.bucketIndex] &= ~(1u << allocInfo.offsetInBucket);
}

bool BitSetAllocator::isAllocated(uint32_t index) {
  auto allocInfo = BucketAllocationInfo(index, N_BITS_IN_UINT);
  return bool(pool_[allocInfo.bucketIndex] & (1 << allocInfo.offsetInBucket));
}

uint32_t BitSetAllocator::getFirstFreeBitPos_(uint32_t bucketIndex) {
  assert(!isBucketFull_(bucketIndex));

#if defined(__GNUC__) || defined(__clang__)
  return __builtin_ctz(~pool_[bucketIndex]);

#elif defined(_MSC_VER)
  unsigned long index;
  _BitScanForward(&index, ~pool_[bucketIndex]);
  return index;

#else
  uint32_t v = ~pool_[bucketIndex];
  static constexpr uint32_t debruijn = 0x03f566f7U;
  static constexpr uint8_t table[32] = {
      0,  1,  12, 2,  13, 22, 17, 3,  14, 33, 23, 36, 18, 58, 28, 4,
      62, 15, 34, 26, 24, 48, 50, 37, 19, 55, 59, 52, 29, 44, 39, 5};
  return table[(v & -v) * debruijn >> 27];
#endif
}

END_VVW_GEN_LIB_NS
