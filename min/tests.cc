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

TEST(MatrixTest, Trivial) {
  Matrix4f m41(Vector4f(1,2,3,4),
      Vector4f(3, 2, 1, 4),
      Vector4f(3, 4, 4, 6),
      Vector4f(2, 3, 5, 5));
  Matrix4f m42(Vector4f(2, -1, 2, 4),
               Vector4f(-3, -1, 3, 9),
               Vector4f(-4, 3, 1, 9),
               Vector4f(0, 0, -1, 2));
  Matrix4f m43(Vector4f(1,2,3,4),
               Vector4f(5, 6, 7, 8),
               Vector4f(9, 10, 11, 12),
               Vector4f(13, 14, 15, 16));
  Matrix3f m31(Vector3f(1, 0, 0),
      Vector3f(-2, 1, 0),
      Vector3f(0, 0, 1));
  Matrix3f m32(Vector3f(1, 0, 6),
              Vector3f(0, 1, 0),
              Vector3f(1, 0, 1));
  auto ret = m41 * m42;
  auto ret2 = m32 * m31;
  std::cout << ret.ToString() << std::endl;
  std::cout << ret2.ToString() << std::endl;
  std::cout << Transpose(m41).ToString() << std::endl;
  std::cout << Inverse(m41).ToString() << std::endl;
}
int main() {
  testing::InitGoogleTest();
  RUN_ALL_TESTS();
  return 0;
}


