#include <Eigen/Core>
#include <iostream>

int main(int, char**) {
  Eigen::Vector3f vec = Eigen::Vector3f(1, 1, 1);
  std::cout << vec;
}
