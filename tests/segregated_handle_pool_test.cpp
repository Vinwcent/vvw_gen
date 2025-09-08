#include <gtest/gtest.h>

#include <vvw_gen/vvw_gen.hpp>
BEGIN_VVW_GEN_LIB_NS

TEST(SegregatedHandlePoolTest, BasicAllocation) {
  SegregatedHandlePool<std::string> pool(100);

  auto index = pool.allocate("key1", 1);
  ASSERT_TRUE(index.has_value());
  EXPECT_EQ(index.value(), 0);
  EXPECT_TRUE(pool.isAllocated("key1"));
  EXPECT_FALSE(pool.isAllocated("key2"));
}

TEST(SegregatedHandlePoolTest, MultipleAllocations) {
  SegregatedHandlePool<std::string> pool(100);

  auto index1 = pool.allocate("key1", 3);
  auto index2 = pool.allocate("key2", 2);
  auto index3 = pool.allocate("key3", 1);

  ASSERT_TRUE(index1.has_value());
  ASSERT_TRUE(index2.has_value());
  ASSERT_TRUE(index3.has_value());

  EXPECT_EQ(index1.value(), 0);
  EXPECT_EQ(index2.value(), 3);
  EXPECT_EQ(index3.value(), 5);

  EXPECT_TRUE(pool.isAllocated("key1"));
  EXPECT_TRUE(pool.isAllocated("key2"));
  EXPECT_TRUE(pool.isAllocated("key3"));
}

TEST(SegregatedHandlePoolTest, DeallocationAndReuse) {
  SegregatedHandlePool<std::string> pool(100);

  auto index1 = pool.allocate("key1", 2);
  auto index2 = pool.allocate("key2", 3);

  pool.deallocate("key1", 2);
  EXPECT_FALSE(pool.isAllocated("key1"));
  EXPECT_TRUE(pool.isAllocated("key2"));

  auto index3 = pool.allocate("key3", 2);
  ASSERT_TRUE(index3.has_value());
  EXPECT_EQ(index3.value(), index1.value());
}

TEST(SegregatedHandlePoolTest, PoolExhaustion) {
  SegregatedHandlePool<int> pool(5);

  auto index1 = pool.allocate(1, 3);
  auto index2 = pool.allocate(2, 2);
  auto index3 = pool.allocate(3, 1);

  EXPECT_TRUE(index1.has_value());
  EXPECT_TRUE(index2.has_value());
  EXPECT_FALSE(index3.has_value());
}

TEST(SegregatedHandlePoolTest, SameSizeReuse) {
  SegregatedHandlePool<std::string> pool(100);

  pool.allocate("key1", 3);
  pool.allocate("key2", 2);
  pool.allocate("key3", 3);

  pool.deallocate("key1", 3);
  pool.deallocate("key3", 3);

  auto index1 = pool.allocate("new1", 3);
  auto index2 = pool.allocate("new2", 3);

  ASSERT_TRUE(index1.has_value());
  ASSERT_TRUE(index2.has_value());
  EXPECT_TRUE(pool.isAllocated("new1"));
  EXPECT_TRUE(pool.isAllocated("new2"));
}

TEST(SegregatedHandlePoolTest, CustomHashingFunctor) {
  struct IntPair {
    int a, b;
    bool operator==(const IntPair& other) const {
      return a == other.a && b == other.b;
    }
  };

  struct IntPairHash {
    size_t operator()(const IntPair& pair) const {
      return std::hash<int>{}(pair.a) ^ (std::hash<int>{}(pair.b) << 1);
    }
  };

  SegregatedHandlePool<IntPair, IntPairHash> pool(100);

  IntPair key1{1, 2};
  IntPair key2{3, 4};

  auto index1 = pool.allocate(key1, 1);
  auto index2 = pool.allocate(key2, 1);

  ASSERT_TRUE(index1.has_value());
  ASSERT_TRUE(index2.has_value());
  EXPECT_TRUE(pool.isAllocated(key1));
  EXPECT_TRUE(pool.isAllocated(key2));
}
END_VVW_GEN_LIB_NS
