#include <min/math/linalg.h>
#include <gtest/gtest.h>

using namespace min;

TEST(VectorTest, Trivial) {
  Vector3f vec(0, 1, 0);
  Vector3f vec3(1, 0, 1);
  Vector4f vec4(1, 2, 3, 4);
  auto ret = Dot(vec, vec3);
  EXPECT_EQ(ret, 0.0f);
}

int main() {
  testing::InitGoogleTest();
  RUN_ALL_TESTS();
  return 0;
}


