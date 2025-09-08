#ifndef VVW_BUCKET_ALLOCATION_INFO_HPP
#define VVW_BUCKET_ALLOCATION_INFO_HPP
#include <cstdint>
struct BucketAllocationInfo {
  uint32_t bucketIndex;
  uint32_t offsetInBucket;

  BucketAllocationInfo(uint32_t index, uint32_t bucketSize)
      : bucketIndex(index / bucketSize), offsetInBucket(index % bucketSize) {}
};
#endif
