#include <gtest/gtest.h>

#include <vvw_gen/vvw_gen.hpp>

TEST(ArgsStorageTest, construction) {
  vvw_gen::ArgsStorage<int, int> storage(5, 4);
  std::tuple<int, int> containedTuple = storage.getArgsTuple();
  EXPECT_EQ(std::get<0>(containedTuple), 5);
  EXPECT_EQ(std::get<1>(containedTuple), 4);

  std::vector<void*> data = storage.getArgPtrs();
  EXPECT_EQ(*static_cast<int*>(data[0]), 5);
  EXPECT_EQ(*static_cast<int*>(data[1]), 4);
}

TEST(ArgsStorageTest, usageInFunctionWrapper) {
  int value = 3;
  std::function<void(int)> valueAdd(
      [&value](int increment) { value += increment; });
  vvw_gen::FunctionWrapper<int> wrapper(valueAdd);

  std::vector<std::unique_ptr<vvw_gen::FunctionWrapperTypeEraser>>
      erasedTypeWrapper;
  erasedTypeWrapper.push_back(
      std::make_unique<vvw_gen::FunctionWrapper<int>>(valueAdd));

  vvw_gen::ArgsStorage<int> storage(3);
  (*erasedTypeWrapper[0])(storage.getArgPtrs());
  EXPECT_EQ(value, 6);
}

TEST(ArgsStorageTest, applyFunction) {
  auto storageTypeErased =
      std::make_unique<vvw_gen::ArgsStorage<int, int>>(5, 4);
  std::vector<void*> data = storageTypeErased->getArgPtrs();
  EXPECT_EQ(*static_cast<int*>(data[0]), 5);
  EXPECT_EQ(*static_cast<int*>(data[1]), 4);

  storageTypeErased->applyFunction(
      std::function<void(int*, int*)>([](int* a, int* b) {
        *a += 1;
        *b += 3;
      }));
  std::vector<void*> data2 = storageTypeErased->getArgPtrs();
  EXPECT_EQ(*static_cast<int*>(data2[0]), 6);
  EXPECT_EQ(*static_cast<int*>(data2[1]), 7);
}
